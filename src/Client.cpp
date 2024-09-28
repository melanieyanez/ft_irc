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

	//std::cout << "Client<" << this << ">: " << fd << std::endl;
	this->fd = fd;
	this->hostname = hostname;
}

Client::~Client()
{
	std::cout << "Client<" << this << ">::~Client: " << fd << std::endl;
	if (close(fd) < 0)
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

std::string Client::readNextPacket()
{
	char buffer[512];
	while (true)
	{
		int read_length = read(fd, buffer, 512);
		if (read_length == -1)
		{
			if (errno == EAGAIN)
				break;
			throw ReadError(errno, *this);
		}
		if (read_length == 0)
			throw ReadError(-1, *this);
		this->line.append(std::string(buffer, read_length));
	}

	std::string::size_type crlf_pos;
	while ((crlf_pos = line.find("\r\n")) != std::string::npos)
	{
		std::string command = line.substr(0, crlf_pos);
		line.erase(0, crlf_pos + 2);
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

void Client::closeConnection() {
	if (fd >= 0) {
		close(fd);  // Fermer le socket
		fd = -1;    // Marquer le socket comme fermé
	}
}