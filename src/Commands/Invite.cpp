#include "Commands/Invite.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Reply.hpp"

Commands::Invite::Invite(std::vector<std::string> command_parts)
{
	this->error = false;

	// Vérification de la syntaxe
	if (command_parts.size() != 3)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}

	this->target = command_parts[1];
	this->channel = command_parts[2];
}

void Commands::Invite::execute(Client& client, Server& server)
{
	Reply reply;

	// Log pour indiquer que la commande INVITE est exécutée pour le client
	client.sendMessage("Executing INVITE command for client: " + client.getNickname(), "console");

	// Si une erreur a été détectée lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, &server, "INVITE", "");
		return;
	}

	// Récupération du canal depuis le serveur
	Channel	*channel = server.getChannel(this->channel);

	// Vérification si le canal existe
	if (!channel)
	{
		reply.sendReply(403, client, NULL, NULL, &server, "INVITE", this->channel);
		return;
	}

	// Vérification si le client est membre du canal
	if (!channel->isMember(client))
	{
		reply.sendReply(442, client, NULL, channel, &server, "INVITE", "");
		return;
	}

	// Vérification si le client est opérateur dans le canal
	if (!channel->isOperator(client))
	{
		reply.sendReply(482, client, NULL, channel, &server, "INVITE", "");
		return;
	}

	// Récupération de l'utilisateur cible par son pseudo
	Client* target = server.getClientByNickname(this->target);

	// Vérification si l'utilisateur cible existe
	if (!target)
	{
		reply.sendReply(401, client, target, channel, &server, "INVITE", this->target);
		return;
	}

	// Vérification si l'utilisateur cible est déjà membre du canal
	if (channel->isMember(*target))
	{
		reply.sendReply(443, client, NULL, channel, &server, "INVITE", target->getNickname());
		return;
	}

	// Invitation de l'utilisateur cible dans le canal
	channel->invite(*target);

	// Envoi de la réponse pour notifier l'invitation réussie
	reply.sendReply(341, client, target, channel, &server, "INVITE");
}