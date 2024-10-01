#include "Commands/Mode.hpp"
#include "Server.hpp"
#include "Reply.hpp"

Commands::Mode::Mode(const std::vector<std::string> &command_parts) : channelName(""), modeMap(), extraParam("")
{
	// Vérification de la syntaxe
	if (command_parts.size() < 2)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}

	this->channelName = command_parts[1];

	if (this->channelName[0] != '#')
	{
		this->error = true;
		this->errorCode = 476;
		return;
	}

	size_t paramIndex = 3; // Les paramètres après le mode
	std::string modes = command_parts[2];

	// Parcourir la chaîne de modes deux caractères à la fois
	for (size_t i = 0; i < modes.size(); i += 2)
	{
		std::string mode = modes.substr(i, 2);
		std::string modeArgument = "";

		// Vérification du format du mode
		if (mode.length() != 2 || (mode[0] != '+' && mode[0] != '-'))
		{
			this->error = true;
			this->errorCode = 472;
			return;
		}

		// Si le mode est +k, -k ou +l, il nécessite un argument
		if (mode == "+k" || mode == "-k" || mode == "+l" || mode == "+o" || mode == "-o")
		{
			if (paramIndex < command_parts.size())
				modeArgument = command_parts[paramIndex++];
			else
			{
				this->error = true;
				this->errorCode = 461;
				return;
			}
		}

		// Stocker le mode et son argument dans la map
		this->modeMap[mode] = modeArgument;
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
		reply.sendReply(this->errorCode, client, NULL, NULL, "MODE", this->channelName);
		return;
	}

	// Recherche du channel spécifié par le client
	Channel* channel = server.getChannel(this->channelName);

	if (!channel)
	{
		reply.sendReply(403, client, NULL, NULL, "MODE", this->channelName);
		return;
	}

	if (this->modeMap.empty())
	{
		// Récupère les modes actifs du canal
		std::string currentModes = channel->getModes();
		
		// Envoie la réponse au client avec les modes actuels
		client.sendBack(":" + server.getHostname() + " 324 " + client.getNickname() + " " + this->channelName + " " + currentModes);
		client.sendMessage("Client " + client.getNickname() + " requested current modes for channel " + this->channelName + ": " + currentModes, "console");
		return;
	}
	else
	{
		// Vérification si l'utilisateur est opérateur dans le channel.
		if (!channel->isOperator(client))
		{
			reply.sendReply(482, client, NULL, channel, "MODE");
			return;
		}
		this->applyModes(client, server, *channel);
	}
}

void Commands::Mode::applyModes(Client &client, Server &server, Channel &channel)
{
	Reply reply;

	// Parcours de chaque mode stocké dans la map (qui contient le mode et l'argument associé)
	for (std::map<std::string, std::string>::iterator it = this->modeMap.begin(); it != this->modeMap.end(); ++it)
	{
		std::string mode = it->first;
		std::string argument = it->second;

		// Vérification pour les modes qui nécessitent un argument
		if ((mode == "+k" || mode == "-k" || mode == "+l" || mode == "-l") && argument.empty())
		{
			// Envoi d'une erreur si un argument est requis mais absent
			reply.sendReply(461, client, NULL, &channel, "MODE", mode);
			continue;
		}

		// ================== Ajout/suppression d'un utilisateur comme opérateur dans un channel (+o/-o)
		if (mode == "+o")
		{
			Client* target = server.getClientByNickname(argument);
			if (!target)
			{
				reply.sendReply(401, client, NULL, &channel, "MODE", argument);
				continue;
			}

			if (channel.isOperator(*target))
			{
				client.sendBack(argument + " is already an operator on " + channel.getChannelName(), "client");
				continue;
			}

			channel.addOperator(*target);
			client.sendBack(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " +o " + argument, "client");
			channel.sendMessage(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " +o " + argument);
		}
		else if (mode == "-o")
		{
			Client* target = server.getClientByNickname(argument);
			if (!target)
			{
				reply.sendReply(401, client, NULL, &channel, "MODE", argument);
				continue;
			}

			if (!channel.isOperator(*target))
			{
				client.sendBack(argument + " is not an operator on " + channel.getChannelName(), "client");
				continue;
			}

			channel.removeOperator(*target);
			client.sendBack(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " -o " + argument, "client");
			channel.sendMessage(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " -o " + argument);
		}

		// ================== Définir/supprimer la limite d’utilisateurs pour le canal (+l / -l)
		else if (mode == "+l")
		{
			int limit = atoi(argument.c_str());
			if (limit <= 0)
			{
				client.sendBack("Invalid limit: Limit must be a positive integer", "client");
				continue;
			}

			channel.setLimits(limit);
			client.sendBack(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " +l " + argument, "client");
			channel.sendMessage(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " +l " + argument);
		}
		else if (mode == "-l")
		{
			channel.setLimits(-1);
			client.sendBack(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " -l", "client");
			channel.sendMessage(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " -l");
		}

		// ================== Définir/supprimer le mot de passe pour le canal (+k / -k)
		else if (mode == "+k")
		{
			if (channel.isProtected())
			{
				reply.sendReply(467, client, NULL, &channel, "MODE");  // 467 : ERR_KEYSET
				continue;
			}

			channel.setPassword(argument);
			channel.setProtected(true);
			client.sendBack(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " +k " + argument, "client");
			channel.sendMessage(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " +k " + argument);
		}
		else if (mode == "-k")
		{
			if (!channel.isCorrectKey(argument))
			{
				reply.sendReply(464, client, NULL, &channel, "MODE", "Incorrect password");
				continue;
			}

			channel.setPassword("");
			channel.setProtected(false);
			client.sendBack(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " -k", "client");
			channel.sendMessage(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " -k");
		}

		// ================== Définir/supprimer le canal sur invitation uniquement (+i/-i)
		else if (mode == "+i")
		{
			if (channel.isInvitationOnly())
			{
				client.sendBack("Mode +i is already applied", "client");
				continue;
			}

			channel.setInvitationOnly(true);
			client.sendBack(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " +i", "client");
			channel.sendMessage(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " +i");
		}
		else if (mode == "-i")
		{
			if (!channel.isInvitationOnly())
			{
				client.sendBack("Mode -i is already applied", "client");
				continue;
			}

			channel.setInvitationOnly(false);
			client.sendBack(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " -i", "client");
			channel.sendMessage(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " -i");
		}

		// ================== Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs (+t/-t)
		else if (mode == "+t")
		{
			if (channel.isTopicRestricted())
			{
				client.sendBack("Mode +t is already applied", "client");
				continue;
			}

			channel.setTopicRestricted(true);
			client.sendBack(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " +t", "client");
			channel.sendMessage(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " +t");
		}
		else if (mode == "-t")
		{
			if (!channel.isTopicRestricted())
			{
				client.sendBack("Mode -t is already applied", "client");
				continue;
			}

			channel.setTopicRestricted(false);
			client.sendBack(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " -t", "client");
			channel.sendMessage(":" + client.getFullIdentifier() + " MODE " + channel.getChannelName() + " -t");
		}
		else
			reply.sendReply(472, client, NULL, &channel, mode);
	}
}
