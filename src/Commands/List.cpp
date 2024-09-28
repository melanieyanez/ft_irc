#include "Commands/List.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Reply.hpp"

#include <iostream>

Commands::List::List(const std::vector<std::string> &command_parts)
{
	// Vérification de la syntaxe
	if (command_parts.size() > 1)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}
}
		
void Commands::List::execute(Client& client, Server& server)
{
	Reply reply;

	// Log dans la console pour indiquer que la commande LIST est exécutée
	client.sendMessage("Executing LIST command for client: " + client.getNickname(), "console");

	// Si une erreur a été détectée lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, "LIST");
		return;
	}

	// Récupération de la liste des canaux disponibles sur le serveur
	std::vector<Channel*> channels = server.getChannels();
	
	// Si aucun canal n'est disponible, log dans la console
	if (channels.empty())
		client.sendMessage("No channels available to list.", "console");

	// Parcours de la liste des canaux pour envoyer les informations de chaque canal
	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel* channel = *it;

		reply.sendReply(322, client, NULL, channel, "LIST");
	}

	// Envoi de la réponse pour indiquer la fin de la liste des canaux
	reply.sendReply(323, client, NULL, NULL, "LIST");
}