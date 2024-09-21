#include "Commands/Invit.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"

Commands::Invite::Invite(std::vector<std::string> command_parts)
{
	if (command_parts.size() != 3)
		throw ;

	this->target = command_parts[1];
	this->channel = command_parts[2];
}

void Commands::Invite::execute(Client& client, Server& server)
{
	Channel* channel = server.getChannel(this->channel);

	// check si le server existe
	if (!channel)
	{
		client.sendMessage("Channel : " + this->channel + " does not exist");
		return;
	}

	// check si la personne existe dans le server quelque part
	Client* target = server.getClientByNickname(this->target);

	// si la personne existe pas = notification
	if (!target)
	{
		client.sendMessage("User : " + this->target + " does not exist");
		return;
	}

	// check si la personne est deja dans le channel
	if (channel->isMember(*target))
	{
		client.sendMessage("User : " + this->target + " is already in the channel");
		return;
	}

	// ajoute la personne au channel
	channel->invite(*target);

	// envois le message d'invitation a la personne
	target->sendBack("INVITE " + channel->getChannelName());

	// envois le message a moi meme pour me dire que l'invitation a ete transmise
	client.sendBack("INVITE " + channel->getChannelName() + " " + target->getNickname());
}