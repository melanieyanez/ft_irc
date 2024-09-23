#include "Commands/User.hpp"
#include "Server.hpp"
#include "Reply.hpp"

#include <iostream>

Commands::User::User(std::vector<std::string> command_parts)
{
	this->error = false;
	
	if (command_parts.size() < 5)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}

	this->name = command_parts[1];

	if (command_parts[4][0] == ':')
	{
		if (command_parts[4].size() == 1)
		{
			this->error = true;
			this->errorCode = 999;
			return;
		}
		else
		{
			this->fullname = command_parts[4].substr(1);
			for (size_t i = 5; i < command_parts.size(); ++i)
				this->fullname += " " + command_parts[i];
		}
	}
	else if (command_parts.size() == 5)
		this->fullname = command_parts[4];
	else
	{
		this->error = true;
		this->errorCode = 999;
	}
}

void Commands::User::execute(Client& client, Server& server)
{
	Reply reply;

	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, &server, "USER");
		return;
	}

	if (!client.hasPass())
	{
		reply.sendReply(451, client, NULL, NULL, &server, "USER");
		return;
	}

	if (!client.hasNick())
	{
		reply.sendReply(431, client, NULL, NULL, &server, "USER");
		return;
	}

	if (client.getIsAuthenticated())
	{
		reply.sendReply(462, client, NULL, NULL, &server, "USER");
		return;
	}

	client.setUsername(name);
	client.setFullname(fullname);

	client.authenticate();
	reply.sendReply(001, client, NULL, NULL, &server, "");
}
