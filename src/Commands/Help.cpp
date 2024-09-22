#include "Commands/Help.hpp"
#include "Client.hpp"
#include "Server.hpp"

Commands::Help::Help(std::vector<std::string> command_parts)
{
	this->error = false;
	if (command_parts.size() > 2)
	{
		this->error = true;
		this->errorMessage = "461 HELP :Syntax error, too many arguments.";
	}
	else if (command_parts.size() == 2)
		this->command = command_parts[1];
	else
		this->command = "";
}

void Commands::Help::execute(Client& client, Server& server)
{
	(void)server;

	if (this->error)
	{
		client.sendBack(this->errorMessage, "client");
		return;
	}

	std::string helpMessage;

	helpMessage += "Help for: ";
	helpMessage += (this->command.empty() ? "General" : this->command);
	helpMessage += "\r\n\r\n";

	if (this->command.empty())
		helpMessage += generateGeneralHelp();
	else
		helpMessage += generateCommandHelp(this->command);

	client.sendBack(helpMessage, "client");
}

std::string Commands::Help::generateGeneralHelp()
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

std::string Commands::Help::generateCommandHelp(const std::string& command)
{
	std::string helpMessage;

	if (command == "JOIN")
	{
		helpMessage += "JOIN <#channel> - Joins the specified channel.\r\n";
		helpMessage += "Usage:\r\n  JOIN #channel\r\n";
		helpMessage += "Details:\r\n  The JOIN command lets you enter a channel. If the channel does not exist, it will be created.\r\n";
		helpMessage += "Examples:\r\n  JOIN #general\r\n  JOIN #help,#support\r\n";
	}
	else if (command == "PRIVMSG")
	{
		helpMessage += "PRIVMSG <nickname|#channel> <message> - Sends a private message.\r\n";
		helpMessage += "Usage:\r\n  PRIVMSG <nickname> <message>\r\n  PRIVMSG #channel <message>\r\n";
		helpMessage += "Examples:\r\n  PRIVMSG Alice Hello there!\r\n  PRIVMSG #general Good morning everyone!\r\n";
	}
	else if (command == "INVITE")
	{
		helpMessage += "INVITE <nickname> <#channel> - Invites a user to the specified channel.\r\n";
		helpMessage += "Usage:\r\n  INVITE <nickname> <#channel>\r\n";
		helpMessage += "Examples:\r\n  INVITE Bob #general\r\n  INVITE Alice #secret\r\n";
	}
	else if (command == "KICK")
	{
		helpMessage += "KICK <#channel> <nickname> [reason] - Kicks a user from the channel.\r\n";
		helpMessage += "Usage:\r\n  KICK <#channel> <nickname> [reason]\r\n";
		helpMessage += "Examples:\r\n  KICK #general troublemaker\r\n  KICK #general troll Being disruptive\r\n";
	}
	else if (command == "ISON")
	{
		helpMessage += "ISON <nickname1> [nickname2 ...] - Checks if the users are online.\r\n";
		helpMessage += "Usage:\r\n  ISON <nickname1> [nickname2 ...]\r\n";
		helpMessage += "Examples:\r\n  ISON Alice Bob Charlie\r\n";
	}
	else if (command == "LIST")
	{
		helpMessage += "LIST - Lists all available channels.\r\n";
		helpMessage += "Usage:\r\n  LIST\r\n";
		helpMessage += "Examples:\r\n  LIST\r\n";
	}
	else if (command == "MODE")
	{
		helpMessage += "MODE <channel|nickname> [modes] - Changes user or channel modes.\r\n";
		helpMessage += "Usage:\r\n  MODE #channel +m\r\n";
		helpMessage += "Examples:\r\n  MODE #general +m\r\n";
	}
	else if (command == "NICK")
	{
		helpMessage += "NICK <newNickname> - Changes your nickname.\r\n";
		helpMessage += "Usage:\r\n  NICK <newNickname>\r\n";
		helpMessage += "Examples:\r\n  NICK newNick\r\n";
	}
	else if (command == "PART")
	{
		helpMessage += "PART <#channel> - Leaves the specified channel.\r\n";
		helpMessage += "Usage:\r\n  PART #channel\r\n";
		helpMessage += "Examples:\r\n  PART #general\r\n";
	}
	else if (command == "PRIVMSG")
	{
		helpMessage += "PRIVMSG <nickname|#channel> <message> - Sends a private message.\r\n";
		helpMessage += "Usage:\r\n  PRIVMSG #channel Hello everyone!\r\n";
	}
	else if (command == "TOPIC")
	{
		helpMessage += "TOPIC <#channel> [new topic] - Changes or views the topic of a channel.\r\n";
		helpMessage += "Usage:\r\n  TOPIC #channel New topic\r\n";
	}
	else if (command == "WHO")
	{
		helpMessage += "WHO [#channel|nickname] - Lists information about users.\r\n";
		helpMessage += "Usage:\r\n  WHO #channel\r\n  WHO nickname\r\n";
	}
	else
	{
		helpMessage += "No detailed help available for the command: " + command + "\r\n";
	}

	return helpMessage;
}
