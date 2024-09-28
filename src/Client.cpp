#include "Client.hpp"
#include "Server.hpp"
#include "Errors.hpp"

#include <unistd.h>
#include <iostream>

Client::Client(Server& server, int fd, std::string hostname) : server(server)
{
	this->isAuthenticated = false;
	this->password = "";
	this->nickname = "";

	// Stockage du descripteur de fichier et du nom d'hôte
	this->fd = fd;
	this->hostname = hostname;
}

Client::~Client()
{
	std::cout << "Client<" << this << ">::~Client: " << fd << std::endl;
	if (close(fd) < 0) // Fermeture du descripteur de fichier
		std::cout << "Client<" << this << ">::~Client: closed failed " << errno << std::endl;
}

std::string Client::getNickname()
{
	if (nickname.empty())
		return "*";
	return nickname;
}

std::string Client::getUsername()
{
	if (username.empty())
		return "*";
	return username;
}

std::string Client::getFullname()
{
	if (fullname.empty())
		return "*";
	return fullname;
}

std::string Client::getPassword()
{
	return password;
}

std::string Client::getFullIdentifier()
{
	return this->getNickname() + "!" + this->getUsername() + "@" + this->server.getHostname();
}

bool Client::getIsAuthenticated() const
{
	return this->isAuthenticated;
}

bool Client::hasNick() const
{
	return !this->nickname.empty();
}

bool Client::hasPass() const
{
	return !this->password.empty();
}

void Client::setNickname(std::string nickname)
{
	this->nickname = nickname;
}

void Client::setUsername(std::string username)
{
	this->username = username;
}

void Client::setFullname(std::string fullname)
{
	this->fullname = fullname;
}

void Client::setPassword(std::string password)
{
	this->password = password;
}

void Client::authenticate()
{
	this->isAuthenticated = true;
}

// Lit les paquets entrants du client et extrait la prochaine commande (jusqu'à "\r\n")
std::string Client::readNextPacket()
{
	char buffer[512];
	while (true)
	{
		// Lecture des données entrantes du client
		int read_length = read(fd, buffer, 512);
		if (read_length == -1)
		{
			if (errno == EAGAIN)
				break; // Pas d'erreur, mais pas encore de données disponibles
			throw ReadError(errno, *this); // En cas d'erreur sérieuse de lecture
		}
		if (read_length == 0) // Si le client a fermé la connexion
			throw ReadError(-1, *this); // Le client est déconnecté
		this->line.append(std::string(buffer, read_length)); // Ajout des données lues au buffer interne
	}

	// Recherche de la commande complète (terminée par "\r\n")
	std::string::size_type crlf_pos;
	while ((crlf_pos = line.find("\r\n")) != std::string::npos)
	{
		std::string command = line.substr(0, crlf_pos); // Extraction de la commande
		line.erase(0, crlf_pos + 2); // Suppression de la commande traitée du buffer interne
		return command;
	}

	return "";
}

void Client::sendBack(std::string reply, std::string target)
{
	reply += "\r\n";
	reply = ":" + server.getHostname() + " " + reply;
	if (target == "client" || target == "both")
		write(fd, reply.c_str(), reply.length());
	if (target == "console" || target == "both")
		std::cout << reply;
}

void Client::sendMessage(std::string message, std::string target)
{
	message += "\r\n";
	if (target == "client" || target == "both")
		write(fd, message.c_str(), message.length());
	if (target == "console" || target == "both")
		std::cout << message;
}

void Client::closeConnection()
{
	if (fd >= 0) // Vérification que le descripteur de fichier est valide
	{
		close(fd); // Fermeture du descripteur de fichier (socket)
		fd = -1; // Marque le socket comme fermé
	}
}