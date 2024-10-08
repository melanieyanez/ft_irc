#include "Commands/Help.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Reply.hpp"

Commands::Help::Help(const std::vector<std::string> &command_parts) : command("")
{
	// Vérification de la syntaxe
	if (command_parts.size() > 2)
	{
		this->error = true;
		this->errorCode = 461;
	}

	// Gestion de la commande optionnelle
	else if (command_parts.size() == 2)
		this->command = command_parts[1];
}

void Commands::Help::execute(Client& client, Server& server)
{
	(void)server;
	Reply reply;

	// Log pour indiquer que la commande HELP est exécutée pour le client
	client.sendMessage("Executing HELP command for client: " + client.getNickname(), "console");

	// Si une erreur a été détectée lors de la construction de la commande
	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, "HELP", "");
		return;
	}
	
	// Vérification si le client est authentifié
	if (!client.getIsAuthenticated())
	{
		// Si aucune commande spécifique n'est demandée, on affiche l'aide pour la connexion
		if (this->command.empty())
		{
			reply.sendReply(704, client, NULL, NULL, "HELP", "");
			client.sendMessage(generateLoginHelp(), "client");
			reply.sendReply(705, client, NULL, NULL, "HELP", "");
			client.sendMessage("Displayed login help for client: " + client.getNickname(), "console");
		}
		else
			// Si le client n'est pas authentifié et demande une commande spécifique, envoi d'une erreur
			reply.sendReply(451, client, NULL, NULL, "HELP", "");
		return;
	}

	// Génération du message d'aide en fonction de la commande fournie
	std::string helpMessage;

	helpMessage += "\r\n\r";

	// Si aucune commande spécifique n'est demandée, on affiche l'aide générale
	if (this->command.empty())
	{
		reply.sendReply(704, client, NULL, NULL, "HELP", "");
		helpMessage += generateGeneralHelp();
		client.sendMessage("Displayed general help for client: " + client.getNickname(), "console");
	}
	else
	{
		// Si une commande spécifique est demandée, on affiche l'aide pour cette commande
		reply.sendReply(704, client, NULL, NULL, "HELP", this->command);
		helpMessage += generateCommandHelp(this->command, client);
	}

	// Envoi du message d'aide au client et de la fin de l'aide
	client.sendMessage(helpMessage, "client");
	reply.sendReply(705, client, NULL, NULL, "HELP", this->command);
}

std::string Commands::Help::generateLoginHelp() const
{
	std::string helpMessage;

	helpMessage += "\nTo use IRC commands, you first need to log in.\r\n";
	helpMessage += "Here are the commands you need to authenticate:\r\n\r\n";

	helpMessage += "PASS <password> - Provide the password for the server.\r\n";
	helpMessage += "NICK <nickname> - Set your nickname.\r\n";
	helpMessage += "USER <username> <hostname> <servername> <realname> - Complete your registration.\r\n\r\n";

	helpMessage += "Once you have completed these steps, you will be logged in and able to use all IRC commands.\r\n";

	return helpMessage;
}

std::string Commands::Help::generateGeneralHelp() const
{
	std::string helpMessage;

	helpMessage += "Available commands:\r\n";
	helpMessage += "JOIN <#channel> - Joins the specified channel.\r\n";
	helpMessage += "PRIVMSG <nickname|#channel> <message> - Sends a private message.\r\n";
	helpMessage += "INVITE <nickname> <#channel> - Invites a user to the specified channel.\r\n";
	helpMessage += "KICK <#channel> <nickname> [reason] - Kicks a user from the channel.\r\n";
	helpMessage += "ISON <nickname1> [nickname2 ...] - Checks if the users are online.\r\n";
	helpMessage += "HELP [command] - Displays help information.\r\n";
	helpMessage += "LIST - Lists all available channels.\r\n";
	helpMessage += "MODE <channel|nickname> [modes] - Changes user or channel modes.\r\n";
	helpMessage += "NICK <newNickname> - Changes your nickname.\r\n";
	helpMessage += "PART <#channel> - Leaves the specified channel.\r\n";
	helpMessage += "TOPIC <#channel> [new topic] - Changes or views the topic of a channel.\r\n";
	helpMessage += "WHO [#channel|nickname] - Lists information about users.\r\n";

	return helpMessage;
}

std::string Commands::Help::generateCommandHelp(const std::string& command, const Client &client) const
{
	std::string helpMessage;

	if (command == "JOIN")
	{
		helpMessage += "JOIN <#channel> - Joins the specified channel.\r\n";
		helpMessage += "Usage:\r\n  JOIN #channel\r\n";
		helpMessage += "Details:\r\n  The JOIN command lets you enter a channel. If the channel does not exist, it will be created.\r\n";
		helpMessage += "Examples:\r\n  JOIN #general\r\n  JOIN #help,#support\r\n";
		client.sendMessage("Displayed help for command: " + this->command + " to client: " + client.getNickname(), "console");
	}
	else if (command == "PRIVMSG")
	{
		helpMessage += "PRIVMSG <nickname|#channel> <message> - Sends a private message.\r\n";
		helpMessage += "Usage:\r\n  PRIVMSG <nickname> <message>\r\n  PRIVMSG #channel <message>\r\n";
		helpMessage += "Examples:\r\n  PRIVMSG Alice Hello there!\r\n  PRIVMSG #general Good morning everyone!\r\n";
		client.sendMessage("Displayed help for command: " + this->command + " to client: " + client.getNickname(), "console");
	}
	else if (command == "INVITE")
	{
		helpMessage += "INVITE <nickname> <#channel> - Invites a user to the specified channel.\r\n";
		helpMessage += "Usage:\r\n  INVITE <nickname> <#channel>\r\n";
		helpMessage += "Examples:\r\n  INVITE Bob #general\r\n  INVITE Alice #secret\r\n";
		client.sendMessage("Displayed help for command: " + this->command + " to client: " + client.getNickname(), "console");
	}
	else if (command == "KICK")
	{
		helpMessage += "KICK <#channel> <nickname> [reason] - Kicks a user from the channel.\r\n";
		helpMessage += "Usage:\r\n  KICK <#channel> <nickname> [reason]\r\n";
		helpMessage += "Examples:\r\n  KICK #general troublemaker\r\n  KICK #general troll Being disruptive\r\n";
		client.sendMessage("Displayed help for command: " + this->command + " to client: " + client.getNickname(), "console");
	}
	else if (command == "ISON")
	{
		helpMessage += "ISON <nickname1> [nickname2 ...] - Checks if the users are online.\r\n";
		helpMessage += "Usage:\r\n  ISON <nickname1> [nickname2 ...]\r\n";
		helpMessage += "Examples:\r\n  ISON Alice Bob Charlie\r\n";
		client.sendMessage("Displayed help for command: " + this->command + " to client: " + client.getNickname(), "console");
	}
	else if (command == "LIST")
	{
		helpMessage += "LIST - Lists all available channels.\r\n";
		helpMessage += "Usage:\r\n  LIST\r\n";
		helpMessage += "Examples:\r\n  LIST\r\n";
		client.sendMessage("Displayed help for command: " + this->command + " to client: " + client.getNickname(), "console");

	}
	else if (command == "MODE")
	{
		helpMessage += "MODE <channel|nickname> [modes] - Changes user or channel modes.\r\n";
		helpMessage += "Usage:\r\n  MODE #channel +m\r\n";
		helpMessage += "Examples:\r\n  MODE #general +m\r\n";
		client.sendMessage("Displayed help for command: " + this->command + " to client: " + client.getNickname(), "console");
	}
	else if (command == "NICK")
	{
		helpMessage += "NICK <newNickname> - Changes your nickname.\r\n";
		helpMessage += "Usage:\r\n  NICK <newNickname>\r\n";
		helpMessage += "Examples:\r\n  NICK newNick\r\n";
		client.sendMessage("Displayed help for command: " + this->command + " to client: " + client.getNickname(), "console");
	}
	else if (command == "PART")
	{
		helpMessage += "PART <#channel> - Leaves the specified channel.\r\n";
		helpMessage += "Usage:\r\n  PART #channel\r\n";
		helpMessage += "Examples:\r\n  PART #general\r\n";
		client.sendMessage("Displayed help for command: " + this->command + " to client: " + client.getNickname(), "console");
	}
	else if (command == "PRIVMSG")
	{
		helpMessage += "PRIVMSG <nickname|#channel> <message> - Sends a private message.\r\n";
		helpMessage += "Usage:\r\n  PRIVMSG #channel Hello everyone!\r\n";
		client.sendMessage("Displayed help for command: " + this->command + " to client: " + client.getNickname(), "console");
	}
	else if (command == "TOPIC")
	{
		helpMessage += "TOPIC <#channel> [new topic] - Changes or views the topic of a channel.\r\n";
		helpMessage += "Usage:\r\n  TOPIC #channel New topic\r\n";
		client.sendMessage("Displayed help for command: " + this->command + " to client: " + client.getNickname(), "console");
	}
	else if (command == "WHO")
	{
		helpMessage += "WHO [#channel|nickname] - Lists information about users.\r\n";
		helpMessage += "Usage:\r\n  WHO #channel\r\n  WHO nickname\r\n";
		client.sendMessage("Displayed help for command: " + this->command + " to client: " + client.getNickname(), "console");
	}
	else
	{
		helpMessage += "No detailed help available for the command: " + command + "\r\n";
		client.sendMessage("Command not found: " + this->command + " for client: " + client.getNickname(), "console");
	}

	return helpMessage;
}
