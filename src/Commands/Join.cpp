#include "Commands/Join.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Reply.hpp"
#include "Commands/Topic.hpp"

#include <sstream>
#include <iostream>
#include <map>

Commands::Join::Join(const std::vector<std::string> &command_parts) : channelName(""), channels(), keys()
{
	// Vérification de la syntaxe
	if (command_parts.size() < 2)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}

	// Séparation des différents noms de canaux fournis
	std::stringstream channelStream(command_parts[1]);
	std::string channel;
	while (std::getline(channelStream, channel, ','))
		this->channels.push_back(channel);

	// Gestion des clés si elles sont fournies
	if (command_parts.size() >= 3)
	{
		std::stringstream keyStream(command_parts[2]);
		std::string key;
		while (std::getline(keyStream, key, ','))
			this->keys.push_back(key);
	}

	// Si le nombre de clés est inférieur au nombre de canaux, remplir avec des chaînes vides
	while (this->keys.size() < this->channels.size())
		this->keys.push_back("");
}

void Commands::Join::execute(Client& client, Server& server)
{
	Reply reply;

	// Log pour indiquer que la commande JOIN est exécutée pour le client
	client.sendMessage("Executing JOIN command for client: " + client.getNickname(), "console");

	// Si une erreur a été détectée lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, "JOIN");
		return;
	}

	// Association entre les canaux et les clés pour chaque tentative de rejoindre un canal
	std::map<std::string, std::string> channelKeyMap;

	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string channelName = channels[i];
		std::string key = keys[i];

		// Vérification du format du nom du canal
		if (channelName[0] != '#')
		{
			reply.sendReply(476, client, NULL, NULL, "JOIN", channelName);
			continue;
		}

		Channel* channel = server.getChannel(channelName);

		// Si le canal est protégé par un mot de passe, on associe la clé fournie
		if (channel && channel->isProtected())
			channelKeyMap[channelName] = key;
		else
			channelKeyMap[channelName] = "";
	}

	// Pour chaque canal, tentative de rejoindre
	for (std::map<std::string, std::string>::iterator it = channelKeyMap.begin(); it != channelKeyMap.end(); ++it)
	{
		std::string channelName = it->first;
		std::string key = it->second;

		// Log pour indiquer que le client tente de rejoindre le canal
		client.sendMessage("Attempting to join channel: " + channelName, "console");

		// Recherche du canal sur le serveur
		Channel* channel = server.getChannel(channelName);

		// Si le canal n'existe pas, le créer et l'ajouter au serveur
		if (!channel)
		{
			client.sendMessage( "Channel not found, creating new channel: " + channelName, "console");
			channel = new Channel(channelName, "");
			server.addChannel(channel);
		}

		// Vérification si le canal est protégé par un mot de passe et que la clé est incorrecte
		if (channel->isProtected() && !channel->isCorrectKey(key))
		{
			reply.sendReply(475, client, NULL, channel, "JOIN");
			continue;
		}

		// Vérification si le client est déjà membre du canal
		if (channel->isMember(client))
		{
			reply.sendReply(443, client, NULL, channel, "JOIN", client.getNickname());
			continue;
		}

		// Vérification si le canal est en mode invitation uniquement et que l'utilisateur n'est pas invité
		if (channel->isInvitationOnly() && !channel->isMember(client) && !channel->isInvited(client))
		{
			reply.sendReply(473, client, NULL, channel, "JOIN");
			continue;
		}

		// Tentative d'ajouter l'utilisateur au canal (échec si le canal est plein)
		if (!channel->addMember(client))
		{
			reply.sendReply(471, client, NULL, channel, "JOIN");
			continue;
		}

		// Log de succès dans la console après avoir rejoint le canal
		client.sendMessage("Success: Client " + client.getNickname() + " joined channel: " + channelName, "console");
		
		// Envoi d'un message aux membres du canal pour indiquer que le client a rejoint
		channel->sendMessage(":" + client.getFullIdentifier() + " JOIN :" + channel->getChannelName());
		
		// Envoi de la liste des utilisateurs dans le canal
		reply.sendReply(353, client, NULL, channel, "JOIN");
		reply.sendReply(366, client, NULL, channel, "JOIN");

		//Envoi du topic du canal
		if (!channel->getTopic().empty())
			reply.sendReply(332, client, NULL, channel, "JOIN");
		else
			reply.sendReply(331, client, NULL, channel, "JOIN");

	}
}