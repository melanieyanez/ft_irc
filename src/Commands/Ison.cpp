#include "Commands/Ison.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Reply.hpp"

#include <sstream>

Commands::Ison::Ison(std::vector<std::string> command_parts)
{
	this->error = false;
	if (command_parts.size() < 2)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}

	std::stringstream ss(command_parts[1]);
	std::string user;
	while (std::getline(ss, user, ' '))
	{
		if (!user.empty())
			this->users.push_back(user);
	}
}

void Commands::Ison::execute(Client& client, Server& server)
{
	Reply reply;

	client.sendMessage("Executing ISON command for client: " + client.getNickname(), "console");

	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, &server, "ISON");
		return;
	}

	std::string onlineUsers;

	for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (server.isNicknameConnected(*it))
			onlineUsers += *it + " ";
	}

	reply.sendReply(303, client, NULL, NULL, &server, "", onlineUsers);
}
