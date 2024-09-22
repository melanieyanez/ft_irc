#include "Commands/Kick.hpp"
#include "Server.hpp"

Commands::Kick::Kick(std::vector<std::string> command_parts)
{
	this->error = false;

	if (command_parts.size() < 3)
	{
		this->error = true;
		this->errorMessage = "999 KICK :Invalid number of parameters.";
		return;
	}
	
	this->channelName = command_parts[1];
	this->nickname = command_parts[2];

	if (command_parts.size() >= 4)
	{
		if (command_parts[3][0] == ':')
		{
		 	this->reason = command_parts[3].substr(1);
			for (size_t i = 4; i < command_parts.size(); ++i)
				this->reason += " " + command_parts[i];
		}
		else
		{
			this->error = true;
			this->errorMessage = "999 KICK :Invalid number of parameters.";
			return;
		}
	}
}

void Commands::Kick::execute(Client& client, Server& server)
{
	if (this->error)
	{
		client.sendBack(this->errorMessage, "client");
		return;
	}

	// recuperer le channel depuis le server
	Channel* channel = server.getChannel(channelName);

	if (!channel)
	{
		client.sendBack("403 " + this->channelName + " :No such channel", "client");
		return;
	}

	if (!channel->isMember(client))
	{
		client.sendBack("442 " + client.getNickname() + " " + channel->getChannelName() + " :You're not on that channel", "client");
		return;
	}

	if (!channel->isOperator(client))
	{
		client.sendBack("482 " + channel->getChannelName() + " :You're not channel operator", "client");
		return;
	}

	// recuperer le nom du mec aupres du server
	Client* target = server.getClientByNickname(this->nickname);

	if (!target)
	{
		client.sendBack("401 " + client.getNickname() + " " + this->nickname + " :No such nick/channel", "client");
		return;
	}

	// check si la personne est dans le channel
	if (!channel->isMember(*target))
	{
		client.sendBack("441 " + client.getNickname() + " " + this->nickname + " " + this->channelName + " :They aren't on that channel", "client");
		return;
	}

	channel->removeMember(*target);

	target->sendBack("KICK " + this->channelName + " " + this->nickname + " :" + this->reason, "client");
	channel->sendBack("KICK " + this->channelName + " " + this->nickname + " :" + this->reason);
}
