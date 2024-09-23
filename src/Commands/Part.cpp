#include "Commands/Part.hpp"
#include "Client.hpp"
#include "Server.hpp"

#include <sstream>

Commands::Part::Part(std::vector<std::string> command_parts)
{
	this->error = false;
	if (command_parts.size() < 2)
	{
		this->error = true;
		this->errorMessage = "461 " + command_parts[0] + " :Not enough parameters";
		return ;
	}

	if (command_parts.size() > 2)
	{
		this->error = true;
		this->errorMessage = "461 " + command_parts[0] + " :Too many parameters (channels must be separated by commas)";
		return;
	}

	std::istringstream ss(command_parts[1]);
	std::string channel;
	while (std::getline(ss, channel, ','))
	{
		if (channel[0] != '#')
		{
			this->error = true;
			this->errorMessage = "476 " + channel + " :Invalid channel name";
			return ;
		}
		this->channels.push_back(channel);
	}
}


void Commands::Part::execute(Client& client, Server& server)
{
	if (this->error)
	{
		std::string fullErrorMessage = this->errorMessage;
		client.sendBack(fullErrorMessage, "client");
		return;
	}

	for (std::vector<std::string>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
	{
		std::string channel_name = *it;
		
		Channel* channel = server.getChannel(channel_name);

		if (!channel) 
		{
			std::string errorMessage = "403 " + client.getNickname() + " " + channel_name + " :No such channel";
			client.sendBack(errorMessage, "client");
			continue;
		}

		if (!channel->isMember(client)) 
		{
			std::string errorMessage = "442 " + client.getNickname() + " " + channel_name + " :You're not on that channel";
			client.sendBack(errorMessage, "client");
			continue;
		}

		channel->removeMember(client);

		std::string partMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getHostname() + " PART " + channel_name;
		channel->sendBack(partMessage);
	}
}

