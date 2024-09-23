#include "Commands/User.hpp"
#include "Server.hpp"

#include <iostream>

Commands::User::User(std::vector<std::string> command_parts)
{
	if (command_parts.size() != 5)
		throw ;

	this->name = command_parts[1];
	this->fullname = command_parts[4];
}

void Commands::User::execute(Client& client, Server& server)
{
	client.setUsername(name);
	client.setFullname(fullname);
	if (server.getPassword() != client.getPassword())
		client.sendBack("464 " + client.getNickname() + " :Password Incorrect");
	else
	{
		client.authenticate();
		client.sendBack("001 " + client.getNickname() + " :Welcome to the Internet Relay Network " + client.getFullIdentifier());
	}
}
