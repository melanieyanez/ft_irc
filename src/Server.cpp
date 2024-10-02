#include "Server.hpp"
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
#include <arpa/inet.h>

// Constructeur de la classe Server, initialisant le port et le mot de passe du serveur
Server::Server(const std::string &port, const std::string &password) : clients_number(0), port(port), password(password), fd(-1), channelName(""), hostname(""), stopRequested(false)
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
		throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(status)));

	// Création du socket pour écouter sur le port
	fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (fd == -1)
	{
		freeaddrinfo(result); // Libération des informations en cas d'erreur
		throw std::runtime_error("socket creation failed: " + std::string(strerror(errno)));
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
		throw std::runtime_error("bind failed: " + std::string(strerror(errno)));
	}

	freeaddrinfo(result); // Libération des informations après liaison réussie

	// Mise en écoute du socket pour accepter les connexions entrantes
	if (listen(fd, 10) == -1)
		throw std::runtime_error("listen failed: " + std::string(strerror(errno)));
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
			throw std::runtime_error("poll error: " + std::string(strerror(errno)));

		// Gestion des nouvelles connexions
		if (fds[0].revents & POLLIN)
		{
			struct sockaddr_storage client_addr; // Stockage de l'adresse du client
			socklen_t client_addr_size = sizeof(client_addr);
			int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_addr_size); // Acceptation de la connexion
			if (client_fd == -1)
				throw std::runtime_error("accept error: " + std::string(strerror(errno)));

			// Récupération du nom d'hôte du client
			struct hostent *host;
			struct sockaddr_in* in_addr = (struct sockaddr_in*)&client_addr;
			host = gethostbyaddr(&(in_addr->sin_addr), sizeof(in_addr->sin_addr), AF_INET);
			
			// Mise en mode non-bloquant du socket client
			if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
				throw std::runtime_error("fcntl error: " + std::string(strerror(errno)));
			this->clients_number++;
			fds[this->clients_number].fd = client_fd;
			fds[this->clients_number].events = POLLIN;
			
			// Déclaration de la variable client
			Client* client = NULL;

			// Si la résolution de l'IP fonctionne, on utilise le nom d'hôte
			if (!host)
			{
				std::string client_ip = inet_ntoa(in_addr->sin_addr);
				client = new Client(*this, client_fd, client_ip);
			}
			else
				client = new Client(*this, client_fd, host->h_name);

			// Création d'un nouvel objet 'Client` pour le client connecté et ajout à la liste des clients
			clients.push_back(client);
			continue;
		}

		// Gestion des événements des clients connectés
		for (int i = this->clients_number; i >= 1; i--)
		{
			try
			{
				if (fds[i].revents & POLLIN) // Si un client a envoyé des données - & (binaire) test le bit POLLIN si il es a un 
				{
					// Lecture de la commande envoyée
					std::string command = clients[i - 1]->readNextPacket();

					// Si le client se déconnecte volontairement
					if (command == "QUIT")
					{
						clients[i - 1]->sendMessage("[" + clients[i - 1]->getFullIdentifier() + "] : " + command, "console");
						clients[i - 1]->sendBack("ERROR :Closing Link: " + clients[i - 1]->getNickname() + " (Client quit)", "client");

						// Enlever le client du serveur
						removeDisconnectedClient(fds, i, this->clients_number);
						this->clients_number--;
						continue;
					}

					// Gestion des autres commandes du client
					handleCommand(command, clients[i - 1]);
				}

				// Gestion de la déconnexion client
				if (fds[i].revents & POLLHUP)
				{
					std::cout << "Client " << clients[i - 1]->getNickname() << " disconnected (POLLHUP)." << std::endl;
					removeDisconnectedClient(fds, i, this->clients_number);
					this->clients_number--;
				}

				// Gestion des erreurs client
				else if (fds[i].revents & POLLERR)
				{
					std::cerr << "Client " << clients[i - 1]->getNickname() << " encountered an error (POLLERR)." << std::endl;

					// Vérifie le code d'erreur pour ignorer les erreurs bénignes
					int error = 0;
					socklen_t errlen = sizeof(error);
					if (getsockopt(fds[i].fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen) == 0)
					{
						if (error == ECONNRESET || error == ETIMEDOUT || error == EPIPE)
						{
							// Erreurs bénignes de connexion
							std::cerr << "Connection error (benign): " << strerror(error) << std::endl;
						}
						else
						{
							std::cerr << "Socket error: " << strerror(error) << std::endl;
						}
					}
					// Suppression du client après une erreur
					removeDisconnectedClient(fds, i, this->clients_number);
					this->clients_number--;
				}
			}
			catch (const std::exception& e)
			{
				removeDisconnectedClient(fds, i, this->clients_number);
				this->clients_number--;
			}
		}
	}
}


void Server::removeDisconnectedClient(struct pollfd fds[], int start_index, int clients_number)
{
	if (start_index <= 0 || start_index > clients_number)
	{
		std::cerr << "Error: Invalid client index for disconnection." << std::endl;
		return;
	}

	Client* disconnectedClient = clients[start_index - 1];

	// Notification aux canaux que le client quitte
	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel* channel = *it;
		if (channel->isMember(*disconnectedClient))
		{
			// Envoi d'un message de départ (QUIT) à tous les membres du canal
			std::string quitMessage = ":" + disconnectedClient->getFullIdentifier() + " QUIT :Client disconnected";
			channel->sendMessage(quitMessage, disconnectedClient);

			// Suppression du client du canal
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
	delete disconnectedClient;
	clients.erase(clients.begin() + (start_index - 1));

	std::cout << "Client successfully removed." << std::endl;
}

void Server::handleCommand(std::string command, Client* creator)
{
	Reply reply;

	if (command.empty() || command == "QUIT")
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
	if (command_parts[0] != "HELP" && command_parts[0] != "PASS" && command_parts[0] != "NICK" && command_parts[0] != "USER" && command_parts[0] != "CAP" && !creator->getIsAuthenticated())
	{
		reply.sendReply(451, *creator, NULL, NULL, command_parts[0]);
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
	else if (command_name == "CAP")
		creator->sendBack("CAP * END");
	else if (command_name == "PONG")
		creator->sendBack(command, "client");
	else
	{
		reply.sendReply(999, *creator, NULL, NULL, command_name);
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

std::string Server::getPassword() const
{
	return this->password;
}

std::vector<Client*> Server::getClients() const
{
	return this->clients;
}

std::vector<Channel*> Server::getChannels() const
{
	return this->channels;
}

// Fonction pour envoyer un message à un destinataire (client ou canal)
void Server::sendMessageToReceiver(const std::string &receiver, const std::string &message, Client& sender)
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

bool Server::isNicknameConnected(const std::string &nickname) const
{
	for (std::vector<Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		Client* client = *it;
		if (client->getNickname() == nickname)
			return true;
	}
	return false;
}

Channel* Server::getChannel(const std::string& channelName) const
{
	for (std::vector<Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it)
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

Client* Server::getClientByNickname(const std::string &nickname) const
{
	for (std::vector<Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it)->getNickname() == nickname)
		{
			return *it;
		}
	}
	return NULL;
}

std::string Server::getHostname() const
{
	return hostname;
}

