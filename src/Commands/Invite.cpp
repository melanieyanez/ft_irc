#include "Commands/Invite.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"

Commands::Invite::Invite(std::vector<std::string> command_parts)
{
	this->error = false;
	if (command_parts.size() != 3)
	{
		this->error = true;
		this->errorMessage = "999 TOPIC :Invalid number of parameters.";
		return;
	}

	this->target = command_parts[1];
	this->channel = command_parts[2];
}

void Commands::Invite::execute(Client& client, Server& server)
{
	if (this->error)
	{
		client.sendBack(this->errorMessage, "client");
		return;
	}

	Channel	*channel = server.getChannel(this->channel);

	// check si le server existe
	if (!channel)
	{
		client.sendBack("403 " + this->channel + " :No such channel", "client");
		return;
	}

	if (!channel->isOperator(client))
	{
		client.sendBack("482 " + channel->getChannelName() + " :You're not channel operator", "client");
		return;
	}

	// check si la personne existe dans le server quelque part
	Client* target = server.getClientByNickname(this->target);

	// si la personne existe pas = notification
	if (!target)
	{
		client.sendBack("401 " + client.getNickname() + " " + this->target + " :No such nick/channel", "client");
		return;
	}

	// check si la personne est deja dans le channel
	if (channel->isMember(*target))
	{
		client.sendBack("443 " + client.getNickname() + " " + this->target + " " + this->channel + " :is already on channel",  "client");
		return;
	}

	// ajoute la personne au channel
	channel->invite(*target);

	// envois le message d'invitation a la personne
	target->sendBack("INVITE " + channel->getChannelName());

	// envois le message a moi meme pour me dire que l'invitation a ete transmise
	client.sendBack("INVITE " + channel->getChannelName() + " " + target->getNickname());
}