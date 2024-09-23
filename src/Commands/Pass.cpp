#include "Commands/Pass.hpp"

#include <iostream>

Commands::Pass::Pass(std::vector<std::string> command_parts)
{
	if (command_parts.size() != 2)
	{
		this->error = true;
		this->errorMessage = "461 PASS :Wrong number of parameters";
		return;
	}
	this->password = command_parts[1];
	this->error = false;
}


void Commands::Pass::execute(Client& client, Server& server)
{
	if (this->error)
	{
		client.sendBack(this->errorMessage, "client");
		return;
	}
	else if (client.hasPass())
	{
		client.sendBack("462 PASS :You may not reregister", "client");
        return;
	}
	else if (server.getPassword() != this->password)
	{
		client.sendBack("464 :Password Incorrect", "client");
		return;
	}
	client.setPassword(password);
}
