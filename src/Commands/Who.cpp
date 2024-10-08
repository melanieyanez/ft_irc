#include "Commands/Who.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Reply.hpp"

Commands::Who::Who(const std::vector<std::string> &command_parts) : Target("")
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

	// Log dans la console pour indiquer que la commande HELP est en cours d'exécution pour le client
	client.sendMessage("Executing WHO command for client: " + client.getNickname(), "console");

	// Gestion des erreurs détectées lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, "WHO");
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

void Commands::Who::listConnectedUsers(Server &server, Client &client) const
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
			reply.sendReply(352, client, connectedClient, NULL, "WHO");
	}

	// Message de fin de la commande /WHO
	reply.sendReply(315, client, NULL, NULL, "WHO");
}
 			
void Commands::Who::listUsersInChannel(Server &server, Client &client, const std::string &channelName) const
{
	Reply reply;

	// Récupérer le canal à partir de son nom
	Channel *channel = server.getChannel(channelName);

	if (channel)
	{
		// Vérifier si le canal est protégé par un mot de passe ou si il est invitation only
		if ((channel->isInvitationOnly() || channel->isProtected()) && !channel->isMember(client))
		{
			reply.sendReply(442, client, NULL, channel, "WHO", channelName);
			return;
		}

		// Récupérer les membres du canal
		std::vector<Client*> members = channel->getMembers();
		
		client.sendMessage("WHO reply for channel: " + channel->getChannelName() + " - Client: " + client.getNickname(), "console");
		for (std::vector<Client*>::iterator it = members.begin(); it != members.end(); ++it)
		{
			Client* channelClient = *it;
			// Format de la réponse pour chaque membre du canal
			if (channelClient != NULL)
				reply.sendReply(352, client, channelClient, channel, "WHO");
		}
	}
	else
	{
		// Si le canal n'existe pas
		reply.sendReply(403, client, NULL, NULL, "WHO", channelName);
		return;
	}
	// Message de fin de la commande /WHO
	reply.sendReply(315, client, NULL, NULL, "WHO");
}
			
void Commands::Who::listSpecificUser(Server &server, Client &client, const std::string &nickName) const
{
	Reply reply;

	// Récupérer un utilisateur spécifique par son pseudonyme
	Client *specificClient = server.getClientByNickname(nickName);

	if (specificClient)
	{
		// Récupérer tous les canaux depuis le serveur
		std::vector<Channel*> channels = server.getChannels();
		
		bool foundInChannel = false;

		// Parcourir tous les channels pour voir si l'utilisateur fait partie d'un channel
		for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
		{
			Channel* channel = *it;
			if (channel->isMember(*specificClient))
			{
				reply.sendReply(352, client, specificClient, *it, "WHO");
				foundInChannel = true;
			}
		}

		// Si l'utilisateur n'était pas dans un channel, on renvoie une réponse générique avec "*"
		if (!foundInChannel)
			reply.sendReply(352, client, specificClient, NULL, "WHO");
	}
	else
	{
		// Si l'utilisateur n'existe pas
		reply.sendReply(401, client, NULL, NULL, "WHO", nickName);
		return;
	}
	// Message de fin de la commande /WHO
	reply.sendReply(315, client, NULL, NULL, "WHO");
}