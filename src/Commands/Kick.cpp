#include "Commands/Kick.hpp"
#include "Server.hpp"
#include "Reply.hpp"

Commands::Kick::Kick(std::vector<std::string> command_parts)
{
	this->error = false;

	if (command_parts.size() < 3)
	{
		this->error = true;
		this->errorCode = 461;
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
			this->errorCode = 461;
			return;
		}
	}
}

void Commands::Kick::execute(Client& client, Server& server)
{
	Reply reply;

	client.sendMessage("Executing KICK command for client: " + client.getNickname(), "console");

	if (this->error)
	{
		reply.sendReply(461, client, NULL, NULL, &server, "KICK");
		return;
	}

	// recuperer le channel depuis le server
	Channel* channel = server.getChannel(channelName);

	if (!channel)
	{
		reply.sendReply(403, client, NULL, NULL, &server, "KICK", channelName);
		return;
	}

	if (!channel->isMember(client))
	{
		reply.sendReply(442, client, NULL, channel, &server, "KICK");
		return;
	}

	if (!channel->isOperator(client))
	{
		reply.sendReply(482, client, NULL, channel, &server, "KICK");
		return;
	}

	// recuperer le nom du mec aupres du server
	Client* target = server.getClientByNickname(this->nickname);

	if (!target)
	{
		reply.sendReply(401, client, NULL, channel, &server, "KICK", this->nickname);
		return;
	}

	// check si la personne est dans le channel
	if (!channel->isMember(*target))
	{
		reply.sendReply(441, client, target, channel, &server, "KICK");
		return;
	}

	channel->removeMember(*target);

	target->sendMessage(":" + client.getFullIdentifier() + " KICK " + channel->getChannelName() + " " + target->getNickname() + " :" + this->reason, "client");
	channel->sendMessage(":" + client.getFullIdentifier() + " KICK " + channel->getChannelName() + " " + target->getNickname() + " :" + this->reason);
}
