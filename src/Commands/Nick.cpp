#include "Commands/Nick.hpp"

Commands::Nick::Nick(std::vector<std::string> command_parts)
{
	if (command_parts.size() != 2)
	{
		this->error = true;
		this->errorMessage = "461 NICK :Wrong number of parameters";
		return;
	}
	this->error = false;
	this->name = command_parts[1];
}

void Commands::Nick::execute(Client& client, Server& server)
{
	if (this->error)
	{
		client.sendBack(this->errorMessage, "client");
		return;
	}
	if (!client.hasPass())
	{
		client.sendBack("451 NICK :You have not registered", "client");
        return;
	}
	if (server.isNicknameConnected(name))
	{
     	client.sendBack("433 " + this->name + " :Nickname is already in use", "client");
        return;
	}
	if (client.getIsAuthenticated())
    {
        client.sendBack(":" + client.getFullIdentifier() + " NICK :" + name);
    }
	client.setNickname(name);
}
