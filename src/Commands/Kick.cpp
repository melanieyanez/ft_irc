#include "Commands/Kick.hpp"
#include "Server.hpp"
#include "Reply.hpp"

Commands::Kick::Kick(std::vector<std::string> command_parts)
{
	this->error = false;

	// Vérification de la syntaxe
	if (command_parts.size() < 3)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}
	
	this->channelName = command_parts[1];
	this->nickname = command_parts[2];

	// Vérification du format du nom du canal
	if (this->channelName[0] != '#')
	{
		this->error = true;
		this->errorCode = 476; // Nom de canal invalide
		return;
	}

	// Gestion optionnelle de la raison
	if (command_parts.size() >= 4)
	{
		// Si la raison commence par un `:`, elle est valide, sinon erreur
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

	// Log pour indiquer que la commande KICK est exécutée pour le client
	client.sendMessage("Executing KICK command for client: " + client.getNickname(), "console");

	// Si une erreur a été détectée lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(461, client, NULL, NULL, &server, "KICK");
		return;
	}

	// Récupération du canal depuis le serveur
	Channel* channel = server.getChannel(channelName);

	// Vérification si le canal existe
	if (!channel)
	{
		reply.sendReply(403, client, NULL, NULL, &server, "KICK", channelName);
		return;
	}

	// Vérification si le client est membre du canal
	if (!channel->isMember(client))
	{
		reply.sendReply(442, client, NULL, channel, &server, "KICK");
		return;
	}

	// Vérification si le client est opérateur dans le canal
	if (!channel->isOperator(client))
	{
		reply.sendReply(482, client, NULL, channel, &server, "KICK");
		return;
	}

	// Récupération du client cible à expulser par son pseudo
	Client* target = server.getClientByNickname(this->nickname);

	// Vérification si l'utilisateur cible existe
	if (!target)
	{
		reply.sendReply(401, client, NULL, channel, &server, "KICK", this->nickname);
		return;
	}

	// Vérification si l'utilisateur cible est membre du canal
	if (!channel->isMember(*target))
	{
		reply.sendReply(441, client, target, channel, &server, "KICK");
		return;
	}

	// Retrait de l'utilisateur du canal
	channel->removeMember(*target);

	// Envoi d'un message à l'utilisateur cible pour lui notifier qu'il a été expulsé
	target->sendMessage(":" + client.getFullIdentifier() + " KICK " + channel->getChannelName() + " " + target->getNickname() + " :" + this->reason, "client");
	
	// Envoi d'un message à tous les membres du canal pour notifier l'expulsion
	channel->sendMessage(":" + client.getFullIdentifier() + " KICK " + channel->getChannelName() + " " + target->getNickname() + " :" + this->reason);
}
