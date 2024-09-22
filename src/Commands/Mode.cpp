#include "Commands/Mode.hpp"
#include "Server.hpp"

Commands::Mode::Mode(std::vector<std::string> command_parts)
{
	this->error = false;
	if (command_parts.size() < 3)
	{
		this->error = true;
		this->errorMessage = "461 MODE :Not enough parameters.";
		return;
	}

	this->channelName = command_parts[1];
	this->mode = command_parts[2];

	if (this->channelName[0] != '#')
	{
		this->error = true;
		this->errorMessage = "403 MODE :Invalid channel name. Channel names must start with '#'.";
		return;
	}

	if (this->mode.length() != 2 || (this->mode[0] != '+' && this->mode[0] != '-'))
	{
		this->error = true;
		this->errorMessage = "461 MODE :Invalid mode syntax. Modes must start with '+' or '-' and be two characters long.";
		return;
	}

	if (mode == "+o" || mode == "-o" || mode == "+l" || mode == "+k" || mode == "-k" )
	{
		if (command_parts.size() != 4)
		{
			this->error = true;
			this->errorMessage = "461 MODE :Wrong number of parameters for mode " + mode;
			return;
		}
		this->extraParam = command_parts[3];
	}

	else if (mode == "+i" || mode == "-i" || mode == "+t" || mode == "-t" || mode == "-l")
	{
		if (command_parts.size() != 3)
		{
			this->error = true;
			this->errorMessage = "461 MODE :Wrong number of parameters for mode " + mode;
			return;
		}
	}
}
	
void Commands::Mode::execute(Client& client, Server& server)
{
	if (this->error)
	{
		client.sendBack(this->errorMessage, "client");
		return;
	}

	Channel* channel = server.getChannel(this->channelName);

	 if (channel == NULL)
	{
		client.sendBack("403 " + channelName + " :No such channel", "client");
		return;
	}
	//si le user n'est pas operator, ne peux rien faire
	if (!channel->isOperator(client))
	{
		client.sendBack("482 " + channelName + " :You're not channel operator", "client");
		return;
	}

	// ================== ajout un user comme operateur dans un channel
	if (mode == "+o")
	{
		Client* target = server.getClientByNickname(extraParam);

		if (target == NULL)
		{
	   		client.sendBack("401 " + client.getNickname() + " " + extraParam + " :No such nick/channel", "client");
			return;
		}

		 if (channel->isOperator(*target))
		{
			client.sendBack(extraParam + " is already an operator on " + channelName, "client");
			return;
		}

		channel->addOperator(*target);

		//confirme au demandeur
		client.sendBack("MODE " + channelName + " +o " + extraParam, "client");
		//informe tous les membres du channel
		channel->sendBack(":" + client.getNickname() + " MODE " + channelName + " +o " + extraParam);
	}

	// supprime un user comme operateur dans un channel
	if (mode == "-o")
	{
		Client* target = server.getClientByNickname(extraParam);
	
		if (target == NULL)
		{
	   		client.sendBack("401 " + client.getNickname() + " " + extraParam + " :No such nick/channel", "client");
			return;
 		}

		if (!channel->isOperator(*target))
		{
			client.sendBack(extraParam + " is not an operator on " + channelName, "client");
			return;
		}

		channel->removeOperator(*target);

		client.sendBack("MODE " + channelName + " -o " + extraParam, "client");
		channel->sendBack(":" + client.getNickname() + " MODE " + channelName + " -o " + extraParam);
	}

	// ================== Définir/supprimer la limite d’utilisateurs pour le canal
	if (mode == "+l")
	{
		try
		{
			int limit = std::stoi(extraParam);
			if (limit <= 0)
			{
				client.sendBack("461 MODE :Invalid limit");
				return;
			}

	   		channel->setLimits(limit);
			client.sendBack("MODE " + channelName + " +l " + extraParam, "client");
			channel->sendBack(":" + client.getNickname() + " MODE " + channelName + " +l " + extraParam);
		}
		catch (const std::invalid_argument& e)
		{
			client.sendBack("461 MODE :Invalid limit - Non-numeric input");
		}
		catch (const std::out_of_range& e)
		{
			client.sendBack("461 MODE :Invalid limit - Number out of range");
		}
	}
	
	if (mode == "-l")
	{
		channel->setLimits(-1);
		client.sendBack("MODE " + channelName + " -l", "client");
		channel->sendBack(":" + client.getNickname() + " MODE " + channelName + " -l");
	}

	// ================== Définir/supprimer le mot de passe pour le canal
	if (mode == "+k")
	{
		// extraParam c'est le parametre[3]
		 if (extraParam.empty())
		{
			client.sendBack("461 MODE :Password required for +k", "client");
			return;
		}
		channel->setPassword(extraParam);
		client.sendBack("MODE " + channelName + " +k " + extraParam, "client");
		channel->sendBack(":" + client.getNickname() + " MODE " + channelName + " +k " + extraParam);
	}
	if (mode == "-k")
	{
		if (extraParam.empty() || !channel->isCorrectKey(extraParam))
		{
			client.sendBack("464 " + channelName + " :Incorrect password");
			return;
		}
		channel->setPassword("");
		client.sendBack("MODE " + channelName + " -k ");
		channel->sendBack(":" + client.getNickname() + " MODE " + channelName + " -k");
	}

	// ================== Définir/supprimer le canal sur invitation uniquement
	if (mode == "+i")
	{
		channel->setInvitationOnly(true);
		client.sendBack("MODE " + channelName + " +i ", "client");
		channel->sendBack(":" + client.getNickname() + " MODE " + channelName + " +i");
	}
	if (mode == "-i")
	{
		channel->setInvitationOnly(false);
		client.sendBack("MODE " + channelName + " -i ", "client");
		channel->sendBack(":" + client.getNickname() + " MODE " + channelName + " -i");
	}

	// ================== Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs de canaux
	if (mode == "+t")
	{
		channel->setTopicRestricted(true);
		client.sendBack("MODE " + channelName + " +t", "client");
    	channel->sendBack(":" + client.getNickname() + " MODE " + channelName + " +t");
	}
	if (mode == "-t")
	{
		channel->setTopicRestricted(false);
		client.sendBack("MODE " + channelName + " -t", "client");
    	channel->sendBack(":" + client.getNickname() + " MODE " + channelName + " -t");
	}
}