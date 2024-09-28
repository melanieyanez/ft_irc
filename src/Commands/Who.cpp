#include "Commands/Who.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Reply.hpp"

Commands::Who::Who(std::vector<std::string> command_parts) : Target("")
{
	// Vérification du nombre de paramètres
	if (command_parts.size() > 2)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}

	// Si aucun paramètre n'est fourni, on liste tous les utilisateurs connectés
	if (command_parts.size() == 1)
		this->Target = "";
	//Sinon, on enregistre le paramètre cible
	else
		this->Target = command_parts[1];
}
			
void Commands::Who::execute(Client& client, Server& server)
{
	Reply reply;

	// Gestion des erreurs détectées lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, &server, "WHO");
		return;
	}
	// Si aucune cible n'est spécifiée, lister tous les utilisateurs connectés
	else if (this->Target.empty())
		listConnectedUsers(server, client);
	// Si la cible est un canal, lister les utilisateurs dans ce canal
	else if (this->Target[0] == '#')
		listUsersInChannel(server, client, this->Target);
	// Sinon, lister l'utilisateur spécifique
	else
		listSpecificUser(server, client, this->Target);
}

void Commands::Who::listConnectedUsers(Server &server, Client &client)
{
	Reply reply;

	// Récupérer tous les utilisateurs connectés au serveur
	std::vector<Client*> clients = server.getClients();

	client.sendMessage("WHO reply for server - Client: " + client.getNickname(), "console");
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		Client* connectedClient = *it;
		// Format de la réponse pour chaque utilisateur connecté
		if (connectedClient != NULL)
			reply.sendReply(352, client, connectedClient, NULL, &server, "WHO", "server");
	}
	// Message de fin de la commande /WHO
	reply.sendReply(315, client, NULL, NULL, &server, "WHO");
}
 			
void Commands::Who::listUsersInChannel(Server &server, Client &client, const std::string &channelName)
{
	Reply reply;

	// Récupérer le canal à partir de son nom
	Channel *channel = server.getChannel(channelName);

	if (channel)
	{
		// Récupérer les membres du canal
		std::vector<Client*> members = channel->getMembers();
		
		client.sendMessage("WHO reply for channel: " + channel->getChannelName() + " - Client: " + client.getNickname(), "console");
		for (std::vector<Client*>::iterator it = members.begin(); it != members.end(); ++it)
		{
			Client* channelClient = *it;
			// Format de la réponse pour chaque membre du canal
			if (channelClient != NULL)
			{
				reply.sendReply(352, client, channelClient, channel, &server, "WHO", "channel");
			}
		}
		// Message de fin de la commande /WHO pour ce canal
		reply.sendReply(315, client, NULL, NULL, &server, "WHO", channelName);
	}
	else
	{
		// Si le canal n'existe pas
		reply.sendReply(403, client, NULL, NULL, &server, "WHO", channelName);
	}
}
			
void Commands::Who::listSpecificUser(Server &server, Client &client, const std::string &nickName)
{
	Reply reply;

	// Récupérer un utilisateur spécifique par son pseudonyme
	Client *specificClient = server.getClientByNickname(nickName);

	if (specificClient)
	{
		client.sendMessage("WHO reply for user: " + specificClient->getNickname() + " - Client: " + client.getNickname(), "console");
		// Format de la réponse pour l'utilisateur spécifique
		reply.sendReply(352, client, specificClient, NULL, &server, "WHO", "user");
	}
	else
	{
		// Si l'utilisateur n'existe pas
		reply.sendReply(401, client, NULL, NULL, &server, "WHO", nickName);
	}
	// Message de fin de la commande /WHO pour cet utilisateur
	reply.sendReply(315, client, NULL, NULL, &server, "WHO");
}