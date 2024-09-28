#include "Commands/Privmsg.hpp"
#include "Server.hpp"
#include "Reply.hpp"

#include <sstream>

Commands::Privmsg::Privmsg(std::vector<std::string> command_parts)
{
	// Définition de la longueur maximale d'un message
	this->MAX_MESSAGE_LENGTH = 400;
	this->error = false;

	// Vérification du nombre de paramètres
	if (command_parts.size() < 3)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}

	bool messageStarted = false;

	// Extraction des destinataires et du message
	std::stringstream recipientStream(command_parts[1]);
	std::string recipient;

	// Gestion des multiples destinataires séparés par des virgules
	while (std::getline(recipientStream, recipient, ','))
		this->recipients.push_back(recipient);

	// Capture du message à envoyer
	for (size_t i = 2; i < command_parts.size(); ++i)
	{
		// Si le message commence avec ':', commencer à le capturer
		if (command_parts[i][0] == ':' && !messageStarted)
		{
			this->message = command_parts[i].substr(1);
			messageStarted = true;
		}
		// Si le message a déjà commencé, ajouter les parties suivantes
		else if (messageStarted)
			this->message += " " + command_parts[i];
		// Si le message ne commence pas avec ":"
		else
		{
			this->error = true;
			this->errorCode = 461;
		}
	}

}

void Commands::Privmsg::execute(Client& client, Server& server)
{	
	Reply reply;

	// Log pour indiquer que la commande PRIVMSG est exécutée pour le client
	client.sendMessage("Executing PRIVMSG command for client: " + client.getNickname(), "console");

	// Gestion des erreurs détectées lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, &server, "PRIVMSG");
		return;
	}

	// Vérification si le message dépasse la longueur maximale
	if (this->message.length() > MAX_MESSAGE_LENGTH)
	{
		client.sendBack("Message too long. Max length is " + std::to_string(MAX_MESSAGE_LENGTH) + " characters", "client");
		client.sendMessage("Client " + client.getNickname() + " sent a message exceeding max length.", "console");
		return;
	}

	// Vérification si le message est vide
	if (this->message.empty())
	{
		client.sendBack("Message is missing", "client");
		client.sendMessage("Client " + client.getNickname() + " attempted to send an empty message.", "console");
		return;
	}

	// Envoi du message à chaque destinataire (canaux ou clients)
	for (std::vector<std::string>::iterator it = recipients.begin(); it != recipients.end(); ++it)
	{
		std::string recipient = *it;

		// Si le destinataire est un canal
		if (recipient[0] == '#')
		{
			// Recherche du channel spécifié par le client
			Channel* channel = server.getChannel(recipient);

			// Vérification si le canal existe
			if (!channel)
			{
				reply.sendReply(403, client, NULL, NULL, &server, "PRIVMSG", recipient);
				continue;
			}

			// Vérification si le client est membre du canal
			if (!channel->isMember(client))
			{
				reply.sendReply(404, client, NULL, channel, &server, "PRIVMSG");
				continue;
			}
			
			// Envoi du message à tous les membres du canal et log dans la console
			channel->sendMessage(":" + client.getFullIdentifier() + " PRIVMSG " + recipient + " :" + message);
			client.sendMessage("Success: Message sent to channel: " + recipient, "console");
		}
		// Sinon, c'est un client cible
		else
		{
			// Récupération de l'utilisateur cible par son pseudo
			Client* target = server.getClientByNickname(recipient);

			// Vérification si le client cible existe
			if (!target)
			{
				reply.sendReply(401, client, NULL, NULL, &server, "PRIVMSG", recipient);
				continue;
			}

			// Envoi du message au client cible et log dans la console
			target->sendMessage(":" + client.getFullIdentifier() + " PRIVMSG " + recipient + " :" + message, "client");
			client.sendMessage("Success: Message sent to client: " + recipient, "console");
		}
	}
}