#include "Commands/User.hpp"
#include "Server.hpp"
#include "Reply.hpp"

#include <iostream>

Commands::User::User(const std::vector<std::string> &command_parts) : name(""), fullname("")
{	
	// Vérification du nombre de paramètres
	if (command_parts.size() < 5)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}

	this->name = command_parts[1];

	// Gestion du fullname qui doit être précédé d'un ':'
	// Si le fullname commence par ':', on récupère tout ce qui suit comme nom complet
	if (command_parts[4][0] == ':')
	{
		// Si le fullname est vide après les deux-points
		if (command_parts[4].size() == 1)
		{
			this->error = true;
			this->errorCode = 461;
			return;
		}
		else
		{
			this->fullname = command_parts[4].substr(1);
			for (size_t i = 5; i < command_parts.size(); ++i)
				this->fullname += " " + command_parts[i];
		}
	}
	// Si le fullname ne commence pas par ':', on considère que le 5e argument est le fullname
	else if (command_parts.size() == 5)
		this->fullname = command_parts[4];
	// Si la syntaxe est incorrecte
	else
	{
		this->error = true;
		this->errorCode = 461;
	}
}

void Commands::User::execute(Client& client, Server& server)
{
	Reply reply;

	// Si une erreur a été détectée lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, &server, "USER");
		return;
	}
	
	// Vérification que le client a envoyé la commande PASS
	if (!client.hasPass())
	{
		reply.sendReply(451, client, NULL, NULL, &server, "USER");
		return;
	}

	// Vérification que le client a un pseudo défini
	if (!client.hasNick())
	{
		reply.sendReply(431, client, NULL, NULL, &server, "USER");
		return;
	}

	// Vérification si le client est déjà authentifié
	if (client.getIsAuthenticated())
	{
		reply.sendReply(462, client, NULL, NULL, &server, "USER");
		return;
	}

	client.setUsername(name);
	client.setFullname(fullname);

	client.authenticate();
	
	// Envoi de la réponse de bienvenue au client
	reply.sendReply(001, client, NULL, NULL, &server, "");
}
