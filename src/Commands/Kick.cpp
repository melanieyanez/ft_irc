#include "Commands/Kick.hpp"
#include "Server.hpp"

Commands::Kick::Kick(std::vector<std::string> command_parts)
{
	if (command_parts.size() < 3 || command_parts.size() > 4)
		throw ;

	this->channelName = command_parts[1];
	this->nickname = command_parts[2];

	if (command_parts.size() == 4)
		this->reason = command_parts[3];
}

void Commands::Kick::execute(Client& client, Server& server)
{
	// recuperer le channel depuis le server
	Channel* channel = server.getChannel(channelName);

	if (!channel)
	{
		client.sendMessage("Channel : " + this->channelName + " does not exist");
		return;
	}

	// a faire : check des droit pour pouvoir le faire
	// pour l'instant tout le monde peux kick tout le monde

	// recuperer le nom du mec aupres du server
	Client* target = server.getClientByNickname(this->nickname);

	// check si la personne existe et est dans le channel
	if (!target || !channel->isMember(*target))
	{
		client.sendMessage("User : " + this->nickname + " is not in the channel");
		return;
	}

	// check si la personne est operateur du channel pour avoir le droit de kick la personne
	if (channel->isOperator(client))
		channel->removeMember(*target);

	// envois le message a tout les membres du channel que la personne a ete kick
	std::vector<Client*> members = channel->getMembers();
	for (std::vector<Client*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		(*it)->sendBack("KICK " + channel->getChannelName() + " " + target->getNickname() + " :" + this->reason);
	}
}
