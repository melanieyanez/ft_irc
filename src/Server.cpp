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

Server::Server(std::string port, std::string password)
{
	this->stopRequested = false;
	this->port = port;
	this->password = password;

	//== prepare =======
	struct addrinfo hints;
	struct addrinfo *result;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE;

	// Résolution de l'adresse locale
	int status = getaddrinfo(NULL, port.c_str(), &hints, &result);
	if (status != 0)
		//std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
		throw std::runtime_error(std::string("getaddrinfo error and errno: "));
	// Création du socket
	fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (fd == -1)
	{
		//std::cerr << "socket error" << std::endl;
		freeaddrinfo(result);
		throw std::runtime_error(std::string("socket error and errno: "));
	}

	// Récupération du nom de la machine ============ ATTENTION ============
	char host_name[1024];
	gethostname(host_name, 1024);
	hostname = host_name;
	std::cout << "Server hostname: " << hostname << std::endl;


	/*
	struct hostent *host;
	host = gethostbyaddr(result, sizeof(struct in_addr), AF_INET);
	if (host == NULL)
	{
		std::cerr << "gethostbyaddr error" << std::endl;
		throw ;
	}
	hostname = host->h_name;
	*/

	fcntl(fd, F_SETFL, O_NONBLOCK);

	// Liaison du socket à une adresse et un port
	if (bind(fd, result->ai_addr, result->ai_addrlen) == -1)
	{
		//std::cerr << "bind error" << std::endl;
		freeaddrinfo(result);
		throw std::runtime_error(std::string("bind error and errno: "));
	}

	freeaddrinfo(result);
	// Écoute des connexions entrantes
	if (listen(fd, 10) == -1)
		//std::cerr << "listen error" << std::endl;
		throw std::runtime_error(std::string("listen error and errno: "));
}

Server::~Server()
{
	close(fd);
}

void Server::stop()
{
	this->stopRequested = true;
}

void Server::start()
{
	struct pollfd fds[1000];
	int clients_number = 0;

	// fds[0] = connection port
	fds[0].fd = this->fd;
	fds[0].events = POLLIN;

	// Server listening
	std::cout << "Server listening on port " << port << std::endl;

	while (!stopRequested)
	{
		if (poll(fds, clients_number + 1, 5000) == -1)
			throw std::runtime_error("poll error");

		// Handle new connections
		if (fds[0].revents & POLLIN)
		{
			struct sockaddr_storage client_addr;
			socklen_t client_addr_size = sizeof(client_addr);
			int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_addr_size);
			if (client_fd == -1)
				throw std::runtime_error("accept error");

			// Get the host name of the client
			struct hostent *host;
			struct sockaddr_in* in_addr = (struct sockaddr_in*)&client_addr;
			host = gethostbyaddr(&(in_addr->sin_addr), sizeof(in_addr->sin_addr), AF_INET);
			//std::cout << "Client hostname: " << host->h_name << std::endl;

			// Set non-blocking mode
			fcntl(client_fd, F_SETFL, O_NONBLOCK);
			clients_number++;
			fds[clients_number].fd = client_fd;
			fds[clients_number].events = POLLIN;

			// Create user and add to clients array
			Client* client = new Client(*this, client_fd, host->h_name);
			clients.push_back(client);
			continue;
		}

		// Handle client events (read data, disconnections, errors)
		for (int i = clients_number; i >= 1; i--)
		{
			try
			{
				if (fds[i].revents & POLLIN)
				{
					// Read and get the command
					std::string command = clients[i - 1]->readNextPacket();

					if (command == "QUIT")
					{
						std::cerr << "Client issued QUIT: " << clients[i - 1]->getNickname() << std::endl;

						clients[i - 1]->sendBack("ERROR :Closing Link: " + clients[i - 1]->getNickname() + " (Client quit)");

						// Remove the client from the server
						removeDisconnectedClient(fds, i, clients_number);
						clients_number--;
						continue;
					}

					// Handle other commands
					handleCommand(command, clients[i - 1]);
				}

				// Handle client disconnection (POLLHUP)
				if (fds[i].revents & POLLHUP)
				{
					std::cerr << "Client disconnected (POLLHUP): " << clients[i - 1]->getNickname() << std::endl;
					removeDisconnectedClient(fds, i, clients_number);
					clients_number--;
				}

				// Handle client error (POLLERR)
				else if (fds[i].revents & POLLERR)
				{
					std::cerr << "Client error (POLLERR): " << clients[i - 1]->getNickname() << std::endl;

					int error = 0;
					socklen_t errlen = sizeof(error);
					getsockopt(fds[i].fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen);
					std::cout << "Error: " << error << std::endl;

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

	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel* channel = *it;
		if (channel->isMember(*disconnectedClient))
		{
			channel->removeMember(*disconnectedClient);
			std::cout << "Client " << disconnectedClient->getNickname() << " removed from channel " << channel->getChannelName() << std::endl;
		}
	}
	// Déplacer les descripteurs de fichiers après la déconnexion
	for (int move_index = start_index; move_index < clients_number; move_index++)
	{
		fds[move_index] = fds[move_index + 1];
	}

	// Supprimer le client de la mémoire et de la liste des clients
	delete clients[start_index - 1];  // fds[i] correspond à clients[i - 1]
	clients.erase(clients.begin() + (start_index - 1));
}

void Server::handleCommand(std::string command, Client* creator)
{
	Reply reply;

	if (command.empty())
		return ;
	std::vector<std::string> command_parts = parseCommand(command);

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
	creator->sendMessage("[" + creator->getFullIdentifier() + "] : " + command, "console");

	std::string command_name = command_parts[0];
	std::transform(command_name.begin(), command_name.end(), command_name.begin(), toupper);

	if (command_parts[0] != "HELP" && command_parts[0] != "PASS" && command_parts[0] != "NICK" && command_parts[0] != "USER" && !creator->getIsAuthenticated())
	{
		reply.sendReply(451, *creator, NULL, NULL, NULL, command_parts[0]);
		return;
	}

	else if (command_name == "PASS")
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
		//if (command_part[0] == ':')
		//{
		//	command.erase(0, 1);
		//	command_parts.push_back(command);
		//	break;
		//}
		//else
		//{
			command.erase(0, space_pos + 1);
			command_parts.push_back(command_part);
		//}
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

void Server::sendMessageToReceiver(std::string receiver, std::string message, Client& sender)
{
	std::cout << "Server::sendMessageToReceiver: sending to clients "  << clients.size() << std::endl;
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		Client* client = *it;
		if (client->getNickname() == receiver)
			client->sendBack(":" + sender.getFullIdentifier() + " PRIVMSG " + receiver + " " + message);
	}

	std::cout << "Server::sendMessageToReceiver: sending to channels " << channels.size() << std::endl;
	// Envois ce message a tout les channel qui va... (sa responsabilite)
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
