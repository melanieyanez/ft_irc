#include "Commands/Pass.hpp"
#include "Reply.hpp"

#include <iostream>

Commands::Pass::Pass(const std::vector<std::string> &command_parts) : password("")
{
	// Vérification du nombre de paramètres
	if (command_parts.size() != 2)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}
	this->password = command_parts[1];
}


void Commands::Pass::execute(Client& client, Server& server)
{
	Reply reply;

	// Gestion des erreurs détectées lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, "PASS");
		return;
	}

	// Vérification si le mot de passe a déjà été défini pour ce client
	else if (client.hasPass())
	{
		reply.sendReply(462, client, NULL, NULL, "PASS");
		return;
	}

	// Vérification si le mot de passe fourni correspond au mot de passe du serveur
	else if (server.getPassword() != this->password)
	{
		reply.sendReply(464, client, NULL, NULL, "PASS");
		return;
	}

	// Définition du mot de passe pour le client
	client.setPassword(password);
}
