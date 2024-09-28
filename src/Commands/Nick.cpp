#include "Commands/Nick.hpp"
#include "Reply.hpp"

Commands::Nick::Nick(std::vector<std::string> command_parts) : name("")
{
	// Vérification de la syntaxe
	if (command_parts.size() != 2)
	{
		this->error = true;
		this->errorCode = 461;		
		return;
	}
	this->name = command_parts[1];
}

void Commands::Nick::execute(Client& client, Server& server)
{
	Reply reply;

	// Si une erreur a été détectée lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, &server, "NICK", "");
		return;
	}

	// Vérification si le client a passé l'étape de la commande PASS
	if (!client.hasPass())
	{
		reply.sendReply(451, client, NULL, NULL, &server, "NICK", "");
		return;
	}

	// Vérification si le pseudo est déjà utilisé par un autre client
	if (server.isNicknameConnected(name))
	{
		reply.sendReply(433, client, NULL, NULL, &server, "NICK", this->name);
		return;
	}

	// Si le client est déjà authentifié, envoi du changement de pseudo à tous les autres utilisateurs
	if (client.getIsAuthenticated())
	{
		// Log pour indiquer que la commande NICK est exécutée pour le client
		client.sendMessage("Executing NICK command for client: " + client.getNickname(), "console");
		
		client.sendMessage("User " + client.getNickname() + " changed nickname to " + name, "console");
		client.sendBack(":" + client.getFullIdentifier() + " NICK :" + name, "client");
	}

	// Mise à jour du pseudo du client
	client.setNickname(name);
}
