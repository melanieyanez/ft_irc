#include "Commands/Topic.hpp"
#include "Reply.hpp"

Commands::Topic::Topic(std::vector<std::string> command_parts) : channel(""), topic(""), isSettingTopic(false)
{
	// Vérification du nombre de paramètres
	if (command_parts.size() < 2)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}
	
	this->channel = command_parts[1];

	// Vérification si le canal commence bien par un '#'
	if (this->channel[0] != '#')
	{
		this->error = true;
		this->errorCode = 476;
		return;
	}

	// Vérification si l'utilisateur essaie de définir un topic
	isSettingTopic = (command_parts.size() >= 3);
	if (isSettingTopic)
	{
		// Si une syntaxe incorrecte est utilisée dans la définition du topic
		if (command_parts.size() > 3 && command_parts[2][0] != ':')
		{
			this->error = true;
			this->errorCode = 461;
			return;
		}

		// Capture du topic avec ou sans les deux points
		if (command_parts[2][0] == ':')
		{
			this->topic = command_parts[2].substr(1);

			for (size_t i = 3; i < command_parts.size(); ++i)
				this->topic += " " + command_parts[i];
		}
		else
			this->topic = command_parts[2];
	}
}

void Commands::Topic::execute(Client& client, Server& server)
{
	Reply reply;

	// Log dans la console pour indiquer que la commande TOPIC est en cours d'exécution pour le client
	client.sendMessage("Executing TOPIC command for client: " + client.getNickname(), "console");

	// Si une erreur a été détectée lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, &server, "TOPIC", this->channel);
		return;
	}

	// Récupération du canal depuis le serveur
	Channel* channel = server.getChannel(this->channel);

	// Si le canal n'existe pas
	if (!channel)
	{
		reply.sendReply(403, client, NULL, NULL, &server, "TOPIC", this->channel);
		return;
	}

	// Si l'utilisateur essaie de définir un nouveau topic ou pas
	if (isSettingTopic)
		setTopic(client, server, *channel);
	else
		getCurrentTopic(client, server, *channel);
}

void Commands::Topic::setTopic(Client& client, Server&, Channel& channel)
{
	Reply reply;

	// Vérification si le canal restreint la modification du topic et si l'utilisateur est opérateur
	if (channel.isTopicRestricted() && !channel.isOperator(client))
	{
		reply.sendReply(482, client, NULL, &channel, NULL, "TOPIC");
		return;
	}

	// Mise à jour du topic dans le canal
	channel.setTopic(this->topic);
	channel.setLastTopicSetter(client.getNickname());
	channel.setLastTopicSetTime();

	// Envoi du nouveau topic à tous les membres du canal
	channel.sendBack(":" + client.getFullIdentifier() + " TOPIC " + channel.getChannelName() + " :" + this->topic);
	client.sendMessage("Success: Topic set for channel: " + channel.getChannelName(), "console");
}

void Commands::Topic::getCurrentTopic(Client& client, Server&, Channel& channel)
{
	Reply reply;

	// Si aucun topic n'est défini pour le canal
	if (channel.getTopic().empty())
		reply.sendReply(331, client, NULL, &channel, NULL, "TOPIC");
	else
	{
		// Envoi du topic actuel et des informations de modification au client
		reply.sendReply(332, client, NULL, &channel, NULL, "TOPIC");
		reply.sendReply(333, client, NULL, &channel, NULL, "TOPIC");
	}
}
