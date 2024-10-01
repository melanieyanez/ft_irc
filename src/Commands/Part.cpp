#include "Commands/Part.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Reply.hpp"

#include <sstream>

Commands::Part::Part(const std::vector<std::string> &command_parts) : channels(), partMessage("")
{
	// Vérification de la syntaxe : au moins un canal doit être spécifié
	if (command_parts.size() < 2)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}

	// Séparation des noms de canaux s'ils sont fournis en liste
	std::istringstream ss(command_parts[1]);
	std::string channel;
	while (std::getline(ss, channel, ','))
		this->channels.push_back(channel);

	// Gestion du message de départ optionnel
	if (command_parts.size() > 2)
	{
		// Si le troisième argument commence par ':', concaténer tout ce qui suit comme message
		if (command_parts[2][0] == ':')
		{
			for (size_t i = 2; i < command_parts.size(); ++i)
			{
				this->partMessage += command_parts[i];
				if (i < command_parts.size() - 1)
					this->partMessage += " ";
			}
			// Supprimer le premier ':' du message
			this->partMessage = this->partMessage.substr(1);
		}
		else
		{
			// Si aucun ':' n'est présent, interpréter le troisième argument comme un seul mot (message de départ)
			this->partMessage = command_parts[2];
		}
	}
}

void Commands::Part::execute(Client& client, Server& server)
{
	Reply reply;

	// Log pour indiquer que la commande PART est exécutée pour le client
	client.sendMessage("Executing PART command for client: " + client.getNickname(), "console");

	// Si une erreur a été détectée lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, "PART");
		return;
	}

	for (std::vector<std::string>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
	{
		std::string channel_name = *it;

		// Vérification si le nom de channel commence par '#'
		if (channel_name[0] != '#')
		{
			reply.sendReply(476, client, NULL, NULL, "PART", channel_name);
			continue;
		}
		
		// Récupération du channel à partir du serveur
		Channel* channel = server.getChannel(channel_name);

		// Vérification si le canal existe
		if (!channel)
		{
			reply.sendReply(403, client, NULL, NULL, "PART", channel_name);
			continue;
		}

		// Vérification si le client est membre du channel
		if (!channel->isMember(client))
		{
			reply.sendReply(442, client, NULL, channel, "PART", "");
			continue;
		}

		// Suppression du client du channel
		channel->removeMember(client);

		// Message de log pour indiquer que le client quitte le canal
		client.sendMessage("Client " + client.getNickname() + " left channel: " + channel_name, "console");

		// Construction du message PART avec un message de départ si fourni
		std::string partMessageToSend = ":" + client.getFullIdentifier() + " PART " + channel_name;
		if (!this->partMessage.empty())
		{
			partMessageToSend += " :" + this->partMessage;
		}

		// Envoi du message PART à tous les membres du channel
		channel->sendMessage(partMessageToSend);
		client.sendMessage(partMessageToSend);
	}
}