#include "Commands/Invite.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Reply.hpp"

Commands::Invite::Invite(std::vector<std::string> command_parts)
{
	this->error = false;
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

	client.sendMessage("Executing INVITE command for client: " + client.getNickname(), "console");

	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, &server, "INVITE", "");
		return;
	}

	Channel	*channel = server.getChannel(this->channel);

	// check si le server existe
	if (!channel)
	{
		reply.sendReply(403, client, NULL, NULL, &server, "INVITE", this->channel);
		return;
	}

	if (!channel->isMember(client))
	{
		reply.sendReply(442, client, NULL, channel, &server, "INVITE", "");
		return;
	}

	if (!channel->isOperator(client))
	{
		reply.sendReply(482, client, NULL, channel, &server, "INVITE", "");
		return;
	}

	// check si la personne existe dans le server quelque part
	Client* target = server.getClientByNickname(this->target);

	// si la personne existe pas = notification
	if (!target)
	{
		reply.sendReply(401, client, target, channel, &server, "INVITE", this->target);
		return;
	}

	// check si la personne est deja dans le channel
	if (channel->isMember(*target))
	{
		reply.sendReply(443, client, NULL, channel, &server, "INVITE", target->getNickname());
		return;
	}

	// ajoute la personne au channel
	channel->invite(*target);

	reply.sendReply(341, client, target, channel, &server, "INVITE", "");
}