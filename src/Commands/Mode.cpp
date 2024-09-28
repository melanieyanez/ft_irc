#include "Commands/Mode.hpp"
#include "Server.hpp"
#include "Reply.hpp"

Commands::Mode::Mode(std::vector<std::string> command_parts) : channelName(""), mode(""), extraParam("")
{
	// Vérification de la syntaxe
	if (command_parts.size() < 3)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}

	this->channelName = command_parts[1];
	this->mode = command_parts[2];

	if (this->channelName[0] != '#')
	{
		this->error = true;
		this->errorCode = 476;
		return;
	}

	// Vérification du format du mode
	if (this->mode.length() != 2 || (this->mode[0] != '+' && this->mode[0] != '-'))
	{
		this->error = true;
		this->errorCode = 472;
		return;
	}

	// Gestion des paramètres en fonction du mode
	if (mode == "+o" || mode == "-o" || mode == "+l" || mode == "+k" || mode == "-k" )
	{
		if (command_parts.size() != 4)
		{
			this->error = true;
			this->errorCode = 461;
			return;
		}
		this->extraParam = command_parts[3];
	}

	else if (mode == "+i" || mode == "-i" || mode == "+t" || mode == "-t" || mode == "-l")
	{
		if (command_parts.size() != 3)
		{
			this->error = true;
			this->errorCode = 461;
			return;
		}
	}
}
	
void Commands::Mode::execute(Client& client, Server& server)
{
	Reply reply;

	// Log dans la console pour indiquer que la commande MODE est en cours d'exécution pour le client
	client.sendMessage("Executing MODE command for client: " + client.getNickname(), "console");

	// Si une erreur a été détectée lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, &server, "MODE", this->channelName);
		return;
	}

	// Recherche du channel spécifié par le client
	Channel* channel = server.getChannel(this->channelName);

	 if (!channel)
	{
		reply.sendReply(403, client, NULL, NULL, &server, "MODE", this->channelName);
		return;
	}

	// Vérification si l'utilisateur est opérateur dans le channel.
	if (!channel->isOperator(client))
	{
		reply.sendReply(482, client, NULL, channel, &server, "MODE");
		return;
	}

	// ================== Ajout/suppression d'un utilisateur comme opérateur dans un channel (+o/-o)
	if (mode == "+o")
	{
		// Recherche du client cible à promouvoir en opérateur
		Client* target = server.getClientByNickname(extraParam);

		// Vérification si le client cible existe
		if (!target)
		{
			reply.sendReply(401, client, NULL, channel, &server, "MODE", extraParam);
			return;
		}

		// Vérification si l'utilisateur est déjà opérateur
		if (channel->isOperator(*target))
		{
			client.sendBack(extraParam + " is already an operator on " + channelName, "client");
			client.sendMessage("Client " + extraParam + " is already an operator on channel " + channelName, "console");
			return;
		}

		// Ajout de l'utilisateur comme opérateur dans le channel
		channel->addOperator(*target);

		client.sendBack(":" + client.getFullIdentifier() + " MODE " + channelName + " +o " + extraParam, "client");
		channel->sendMessage( ":" + client.getFullIdentifier() + " MODE " + channelName + " +o " + extraParam);
		client.sendMessage("Success: Client " + target->getNickname() + " is now an operator on channel " + channelName, "console");
	}

	else if (mode == "-o")
	{
		// Recherche du client cible à rétrograder
		Client* target = server.getClientByNickname(extraParam);
	
		// Vérification si le client cible existe
		if (!target)
		{
			reply.sendReply(401, client, NULL, channel, &server, "MODE", extraParam);
			return;
 		}

		// Vérification si l'utilisateur est opérateur
		if (!channel->isOperator(*target))
		{
			client.sendBack(extraParam + " is not an operator on " + channelName, "client");
			client.sendMessage("Client " + extraParam + " is not an operator on channel " + channelName, "console");
			return;
		}

		// Suppression de l'utilisateur en tant qu'opérateur du channel
		channel->removeOperator(*target);

		client.sendBack("MODE " + channelName + " -o " + extraParam, "client");
		channel->sendMessage(":" + client.getFullIdentifier() + " MODE " + channelName + " -o " + extraParam);
		client.sendMessage("Success: Client " + target->getNickname() + " is no longer an operator on channel " + channelName, "console");
	}

	// ================== Définir/supprimer la limite d’utilisateurs pour le canal (+l / -l)
	else if (mode == "+l")
	{
		try
		{
			// Conversion du paramètre extra en entier
			int limit = std::stoi(extraParam);
			if (limit <= 0)
			{
				client.sendBack("Invalid limit: Limit must be a positive integer", "client");
				client.sendMessage("Client " + client.getNickname() + " attempted to set an invalid limit on channel " + channelName, "console");
				return;
			}

			// Si une limite est déjà définie, log du changement dans la console
			if (channel->getLimits() > 0)
				client.sendMessage("Success: Channel " + channelName + " limit modified to " + extraParam, "console");
			// Sinon, log que la limite a été définie pour la première fois
			else
				client.sendMessage("Success: Channel " + channelName + " limit set to " + extraParam, "console");
	   		
			// Mise à jour de la limite sur le channel
			channel->setLimits(limit);
			client.sendBack("MODE " + channelName + " +l " + extraParam, "client");
			channel->sendMessage(":" + client.getFullIdentifier() + " MODE " + channelName + " +l " + extraParam);
		}
		catch (const std::invalid_argument& e)
		{
			// Gestion d'une limite non-numérique
			client.sendBack("Invalid limit: Non-numeric input provided", "client");
			client.sendMessage("Client " + client.getNickname() + " provided a non-numeric limit for channel " + channelName, "console");
		}
		catch (const std::out_of_range& e)
		{
			// Gestion d'une limite trop élevée
			client.sendBack("Invalid limit: Number out of range", "client");
			client.sendMessage("Error: Client " + client.getNickname() + " provided a limit out of range for channel " + channelName, "console");
		}
	}
	
	else if (mode == "-l")
	{
		//Vérification si le channel a une limite
		if (channel->getLimits() == -1)
		{
			client.sendMessage("No limit is currently set for channel " + channelName, "client");
			client.sendMessage("Channel " + channelName + " has no limit set", "console");
		}
		else
		{
			// Retrait de la limite
			channel->setLimits(-1);
			client.sendBack("MODE " + channelName + " -l", "client");
			channel->sendMessage(":" + client.getFullIdentifier() + " MODE " + channelName + " -l");
			client.sendMessage("Success: Channel " + channelName + " limit removed", "console");
		}
	}

	// ================== Définir/supprimer le mot de passe pour le canal (+k / -k)
	else if (mode == "+k")
	{
		// Vérification si le canal a déjà un mot de passe défini
		if (channel->isProtected())
		{
			reply.sendReply(467, client, NULL, channel, &server, "MODE");
			return;
		}
		
		// Définition du mot de passe pour le canal
		channel->setPassword(extraParam);
		channel->setProtected(true);
		client.sendBack("MODE " + channelName + " +k " + extraParam, "client");
		channel->sendMessage(":" + client.getFullIdentifier() + " MODE " + channelName + " +k " + extraParam);
		client.sendMessage("Success: Password set for channel " + channelName, "console");
	}
	else if (mode == "-k")
	{
		//Vérification si le mot de passe fourni est correct
		if (extraParam.empty() || !channel->isCorrectKey(extraParam))
		{
			reply.sendReply(464, client, NULL, channel, &server, "MODE", "Incorrect password");
			return;
		}
		
		// Retrait du mot de passe du canal
		channel->setPassword("");
		channel->setProtected(false);
		client.sendBack("MODE " + channelName + " -k", "client");
		channel->sendMessage(":" + client.getFullIdentifier() + " MODE " + channelName + " -k");
		client.sendMessage("Success: Password removed from channel " + channelName, "console");
	}

	// ================== Définir/supprimer le canal sur invitation uniquement (+i/-i)
	else if (mode == "+i")
	{
		// Vérification si le mode est déjà appliqué
		 if (channel->isInvitationOnly())
		{
			client.sendBack("Mode +i is already applied", "client");
			client.sendMessage("Mode +i is already applied on channel " + channelName, "console");
			return;
		}

		// Application du mode sur invitation
		channel->setInvitationOnly(true);
		client.sendBack("MODE " + channelName + " +i", "client");
		channel->sendMessage(":" + client.getFullIdentifier() + " MODE " + channelName + " +i");
		client.sendMessage("Success: Channel " + channelName + " is now invite-only", "console");
	}
	else if (mode == "-i")
	{
		// Vérification si le mode est déjà appliqué
		if (!channel->isInvitationOnly())
		{
			client.sendBack("Mode -i is already applied", "client");
			client.sendMessage("Mode -i is already applied on channel " + channelName, "console");
			return;
		}

		// Retrait du mode sur invitation
		channel->setInvitationOnly(false);
		client.sendBack("MODE " + channelName + " -i", "client");
		channel->sendMessage(":" + client.getFullIdentifier() + " MODE " + channelName + " -i");
		client.sendMessage("Success: Channel " + channelName + " is no longer invite-only", "console");
	}

	// ================== Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs (+t/-t)
	else if (mode == "+t")
	{
		// Vérification si le mode est déjà appliqué
		if (channel->isTopicRestricted())
		{
			client.sendBack("Mode +t is already applied", "client");
			client.sendMessage("Mode +t is already applied on channel " + channelName, "console");
			return;
		}

		// Application de la restriction de sujet
		channel->setTopicRestricted(true);
		client.sendBack("MODE " + channelName + " +t", "client");
		channel->sendMessage(":" + client.getFullIdentifier() + " MODE " + channelName + " +t");
		client.sendMessage("Success: Topic restrictions enabled for channel " + channelName, "console");
	}
	else if (mode == "-t")
	{
		// Vérification si le mode est déjà appliqué
		if (!channel->isTopicRestricted())
		{
			client.sendBack("Mode -t is already applied", "client");
			client.sendMessage("Mode -t is already applied on channel " + channelName, "console");
			return;
		}

		// Retrait de la restriction de sujet
		channel->setTopicRestricted(false);
		client.sendBack("MODE " + channelName + " -t", "client");
		channel->sendMessage(":" + client.getFullIdentifier() + " MODE " + channelName + " -t");
		client.sendMessage("Success: Topic restrictions disabled for channel " + channelName, "console");
	}
	// Si le mode demandé est invalide
	else
	{
		reply.sendReply(472, client, NULL, channel, &server, mode);
	}
}