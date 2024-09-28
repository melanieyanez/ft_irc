#include "Commands/Ison.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Reply.hpp"

#include <sstream>

Commands::Ison::Ison(std::vector<std::string> command_parts) : users()
{
	// Vérification de la syntaxe
	if (command_parts.size() < 2)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}

	// Séparation des noms d'utilisateurs à partir du deuxième paramètre
	std::stringstream ss(command_parts[1]);
	std::string user;
	while (std::getline(ss, user, ' '))
	{
		if (!user.empty())
			this->users.push_back(user);
	}
}

void Commands::Ison::execute(Client& client, Server& server)
{
	Reply reply;

	// Log pour indiquer que la commande ISON est exécutée pour le client
	client.sendMessage("Executing ISON command for client: " + client.getNickname(), "console");

	// Si une erreur a été détectée lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, &server, "ISON");
		return;
	}

	// Chaîne pour stocker les utilisateurs qui sont en ligne
	std::string onlineUsers;

	// Parcours de la liste des utilisateurs fournie par le client
	for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); ++it)
	{
		// Si l'utilisateur est connecté, on l'ajoute à la liste des utilisateurs en ligne
		if (server.isNicknameConnected(*it))
			onlineUsers += *it + " ";
	}

	// Envoi de la réponse avec la liste des utilisateurs connectés
	reply.sendReply(303, client, NULL, NULL, &server, "", onlineUsers);
}
