#include "Commands/Ison.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <sstream>

Commands::Ison::Ison(std::vector<std::string> command_parts)
{
	this->error = false;
	if (command_parts.size() < 2)
	{
		this->error = true;
		this->errorMessage = "461 ISON :Not enough parameters.";
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
	if (this->error)
	{
		client.sendBack(this->errorMessage, "client");
		return;
	}

	std::string response = "303 " + client.getNickname() + " :";

	for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (server.isNicknameConnected(*it))
			response += *it + " ";
	}

	client.sendBack(response, "client");
}
