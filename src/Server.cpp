#include "Server.hpp"
#include "Errors.hpp"
#include "Reply.hpp"
#include "Commands/Nick.hpp"
#include "Commands/User.hpp"
#include "Commands/Pass.hpp"
#include "Commands/Privmsg.hpp"
#include "Commands/Ison.hpp"
#include "Commands/Join.hpp"
#include "Commands/Kick.hpp"
#include "Commands/Topic.hpp"
#include "Commands/Mode.hpp"
#include "Commands/Invite.hpp"
#include "Commands/Help.hpp"
#include "Commands/Who.hpp"
#include "Commands/List.hpp"
#include "Commands/Part.hpp"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>
#include <sys/fcntl.h>
#include <string>
#include <sstream>

// Constructeur de la classe Server, initialisant le port et le mot de passe du serveur
Server::Server(std::string port, std::string password) : port(port), password(password), fd(-1), channelName(""), hostname(""), stopRequested(false)
{
	// Préparation de l'initialisation du socket avec les informations sur l'adresse et le port
	struct addrinfo hints;
	struct addrinfo *result;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET; // Utilisation du protocole IPv4
	hints.ai_socktype = SOCK_STREAM; // Utilisation du protocole TCP
	hints.ai_flags = AI_PASSIVE; // Acceptation de toute adresse

	// Résolution de l'adresse locale pour obtenir les informations nécessaires à la création du socket
	int status = getaddrinfo(NULL, port.c_str(), &hints, &result);
	if (status != 0)
		throw std::runtime_error(std::string("getaddrinfo error and errno: "));

	// Création du socket pour écouter sur le port
	fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (fd == -1)
	{
		freeaddrinfo(result); // Libération des informations en cas d'erreur
		throw std::runtime_error(std::string("socket error and errno: "));
	}

	// Récupération du nom d'hôte de la machine pour affichage
	char host_name[1024];
	gethostname(host_name, 1024); // Remplissage du tableau host_name avec le nom de la machine
	hostname = host_name;
	std::cout << "Server hostname: " << hostname << std::endl;

	// Mise en mode non-bloquant du socket
	fcntl(fd, F_SETFL, O_NONBLOCK);

	// Liaison du socket à une adresse et un port
	if (bind(fd, result->ai_addr, result->ai_addrlen) == -1)
	{
		freeaddrinfo(result); // Libération des informations en cas d'erreur
		throw std::runtime_error(std::string("bind error and errno: "));
	}

	freeaddrinfo(result); // Libération des informations après liaison réussie

	// Mise en écoute du socket pour accepter les connexions entrantes
	if (listen(fd, 10) == -1)
		throw std::runtime_error(std::string("listen error and errno: "));
}

// Destructeur du serveur qui ferme le socket
Server::~Server()
{
	close(fd);
}

void Server::stop()
{
	this->stopRequested = true;
}

// Fonction principale pour démarrer le serveur et gérer les connexions et les événements des clients
void Server::start()
{
	struct pollfd fds[1000]; // Tableau de pollfd pour surveiller jusqu'à 1000 descripteurs de fichier
	int clients_number = 0;

	// Configuration initiale : le descripteur fds[0] est réservé pour les connexions entrantes (socket principal)
	fds[0].fd = this->fd;
	fds[0].events = POLLIN; // On surveille les événements d'entrée

	// Server listening
	std::cout << "Server listening on port " << port << std::endl;

	// Boucle principale du serveur
	while (!stopRequested)
	{
		// Utilisation de `poll` pour surveiller les événements sur les descripteurs de fichiers
		if (poll(fds, clients_number + 1, 5000) == -1)
			throw std::runtime_error("poll error");

		// Gestion des nouvelles connexions
		if (fds[0].revents & POLLIN)
		{
			struct sockaddr_storage client_addr; // Stockage de l'adresse du client
			socklen_t client_addr_size = sizeof(client_addr);
			int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_addr_size); // Acceptation de la connexion
			if (client_fd == -1)
				throw std::runtime_error("accept error");

			// Récupération du nom d'hôte du client
			struct hostent *host;
			struct sockaddr_in* in_addr = (struct sockaddr_in*)&client_addr;
			host = gethostbyaddr(&(in_addr->sin_addr), sizeof(in_addr->sin_addr), AF_INET);

			// Mise en mode non-bloquant du socket client
			fcntl(client_fd, F_SETFL, O_NONBLOCK);
			clients_number++;
			fds[clients_number].fd = client_fd;
			fds[clients_number].events = POLLIN;

			// Création d'un nouvel objet 'Client` pour le client connecté et ajout à la liste des clients
			Client* client = new Client(*this, client_fd, host->h_name);
			clients.push_back(client);
			continue;
		}

		// Gestion des événements des clients connectés
		for (int i = clients_number; i >= 1; i--)
		{
			try
			{
				if (fds[i].revents & POLLIN) // Si un client a envoyé des données
				{
					// Lecture de la commande envoyée
					std::string command = clients[i - 1]->readNextPacket();

					// Si le client se déconnecte volontairement
					if (command == "QUIT")
					{
						//std::cerr << "Client issued QUIT: " << clients[i - 1]->getNickname() << std::endl;
						clients[i - 1]->sendBack("ERROR :Closing Link: " + clients[i - 1]->getNickname() + " (Client quit)");

						// Enlever le client du serveur
						removeDisconnectedClient(fds, i, clients_number);
						clients_number--;
						continue;
					}

					// Gestion des autres commandes du client
					handleCommand(command, clients[i - 1]);
				}

				// Gestion de la déconnexion client
				if (fds[i].revents & POLLHUP)
				{
					//std::cerr << "Client disconnected (POLLHUP): " << clients[i - 1]->getNickname() << std::endl;
					removeDisconnectedClient(fds, i, clients_number);
					clients_number--;
				}

				// Gestion des erreurs client
				else if (fds[i].revents & POLLERR)
				{
					//std::cerr << "Client error (POLLERR): " << clients[i - 1]->getNickname() << std::endl;

					//int error = 0;
					//socklen_t errlen = sizeof(error);
					//getsockopt(fds[i].fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen);
					//std::cout << "Error: " << error << std::endl;

					removeDisconnectedClient(fds, i, clients_number);
					clients_number--;
				}
			}
			catch (ReadError& exception)
			{
				std::cerr << "ReadError: Client disconnected unexpectedly: " << exception.getClient().getNickname() << std::endl;
				removeDisconnectedClient(fds, i, clients_number);
				clients_number--;
			}
		}
	}
}

void Server::removeDisconnectedClient(struct pollfd fds[], int start_index, int clients_number)
{
	Client* disconnectedClient = clients[start_index - 1];

	// Suppression du client de tous les canaux dont il est membre
	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel* channel = *it;
		if (channel->isMember(*disconnectedClient))
		{
			channel->removeMember(*disconnectedClient);
			std::cout << "Client " << disconnectedClient->getNickname() << " removed from channel " << channel->getChannelName() << std::endl;
		}
	}
	// Réorganisation des descripteurs de fichiers
	for (int move_index = start_index; move_index < clients_number; move_index++)
	{
		fds[move_index] = fds[move_index + 1];
	}

	// Suppression du client de la mémoire et de la liste des clients
	delete clients[start_index - 1];
	clients.erase(clients.begin() + (start_index - 1));
}

void Server::handleCommand(std::string command, Client* creator)
{
	Reply reply;

	if (command.empty())
		return ;
	std::vector<std::string> command_parts = parseCommand(command);

	// Traitement de certaines commandes sensibles (masquage de mots de passe)
	if (command_parts[0] == "PASS")
	{
		size_t passPos = command.find(command_parts[1]);
		if (passPos != std::string::npos)
			command.replace(passPos, command_parts[1].length(), std::string(command_parts[1].length(), '*'));
	}
	else if (command_parts[0] == "JOIN" && command_parts.size() > 2)
	{
		size_t keyPos = command.find(command_parts[2]);
		if (keyPos != std::string::npos)
		{
			std::stringstream keyStream(command_parts[2]);
			std::string key;
			while (std::getline(keyStream, key, ','))
			{
				command.replace(keyPos, key.length(), std::string(key.length(), '*'));
				keyPos += key.length() + 1;
			}
		}
	}
	else if (command_parts[0] == "MODE" && command_parts.size() > 3 && (command_parts[2] == "+k" || command_parts[2] == "-k"))
	{
		size_t passPos = command.find(command_parts[3]);
		if (passPos != std::string::npos)
		{
			command.replace(passPos, command_parts[3].length(), std::string(command_parts[3].length(), '*'));
		}
	}
	
	// Envoi de la commande dans la console pour affichage
	creator->sendMessage("[" + creator->getFullIdentifier() + "] : " + command, "console");

	std::string command_name = command_parts[0];
	std::transform(command_name.begin(), command_name.end(), command_name.begin(), toupper);

	// Vérification si le client est authentifié pour exécuter certaines commandes
	if (command_parts[0] != "HELP" && command_parts[0] != "PASS" && command_parts[0] != "NICK" && command_parts[0] != "USER" && !creator->getIsAuthenticated())
	{
		reply.sendReply(451, *creator, NULL, NULL, NULL, command_parts[0]);
		return;
	}

	// Exécution des commandes selon leur nom
	if (command_name == "PASS")
		Commands::Pass(command_parts).execute(*creator, *this);
	else if (command_name == "NICK")
		Commands::Nick(command_parts).execute(*creator, *this);
	else if (command_name == "USER")
		Commands::User(command_parts).execute(*creator, *this);
	else if (command_name == "PRIVMSG")
		Commands::Privmsg(command_parts).execute(*creator, *this);
	else if (command_name == "ISON")
		Commands::Ison(command_parts).execute(*creator, *this);
	else if (command_name == "JOIN")
		Commands::Join(command_parts).execute(*creator, *this);
	else if (command_name == "KICK")
		Commands::Kick(command_parts).execute(*creator, *this);
	else if (command_name == "TOPIC")
		Commands::Topic(command_parts).execute(*creator, *this);
	else if (command_name == "MODE")
		Commands::Mode(command_parts).execute(*creator, *this);
	else if (command_name == "INVITE")
		Commands::Invite(command_parts).execute(*creator, *this);
	else if (command_name == "HELP")
		Commands::Help(command_parts).execute(*creator, *this);
	else if (command_name == "WHO")
		Commands::Who(command_parts).execute(*creator, *this);
	else if (command_name == "LIST")
		Commands::List(command_parts).execute(*creator, *this);
	else if (command_name == "PART")
		Commands::Part(command_parts).execute(*creator, *this);
	else
	{
		reply.sendReply(999, *creator, NULL, NULL, NULL, command_name);
	}
}

// Fonction pour analyser la commande et la diviser en plusieurs parties
std::vector<std::string> Server::parseCommand(std::string command)
{
	std::string::size_type space_pos;
	std::string command_part;
	std::vector<std::string> command_parts;

	while (!command.empty())
	{
		space_pos = command.find(' ');
		if (space_pos == std::string::npos)
			space_pos = command.size();
		command_part = command.substr(0, space_pos);
		command.erase(0, space_pos + 1);
		command_parts.push_back(command_part);
	}
	return command_parts;
}

std::string Server::getPassword()
{
	return this->password;
}

std::vector<Client*> Server::getClients()
{
	return this->clients;
}

std::vector<Channel*> Server::getChannels()
{
	return this->channels;
}

// Fonction pour envoyer un message à un destinataire (client ou canal)
void Server::sendMessageToReceiver(std::string receiver, std::string message, Client& sender)
{
	// Envoi du message aux clients
	std::cout << "Server::sendMessageToReceiver: sending to clients "  << clients.size() << std::endl;
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		Client* client = *it;
		if (client->getNickname() == receiver)
			client->sendBack(":" + sender.getFullIdentifier() + " PRIVMSG " + receiver + " " + message);
	}
	
	// Envoi du message aux canaux
	std::cout << "Server::sendMessageToReceiver: sending to channels " << channels.size() << std::endl;
	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel* channel = *it;
		if (channel->getChannelName() == receiver)
		{
			if (channel->isMember(sender))
				channel->sendBack(":" + sender.getFullIdentifier() + " PRIVMSG " + receiver + " " + message);
		}
	}
}

bool Server::isNicknameConnected(std::string nickname)
{
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		Client* client = *it;
		if (client->getNickname() == nickname)
			return true;
	}
	return false;
}

Channel* Server::getChannel(const std::string& channelName)
{
	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		if ((*it)->getChannelName() == channelName)
		{
			return *it;
		}
	}
	return NULL;
}

void Server::addChannel(Channel* channel)
{
	channels.push_back(channel);
}

Client* Server::getClientByNickname(std::string nickname)
{
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it)->getNickname() == nickname)
		{
			return *it;
		}
	}
	return NULL;
}

std::string Server::getHostname()
{
	return hostname;
}
