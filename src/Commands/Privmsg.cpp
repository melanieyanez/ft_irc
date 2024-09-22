#include "Commands/Privmsg.hpp"
#include "Server.hpp"
#include <sstream>

Commands::Privmsg::Privmsg(std::vector<std::string> command_parts)
{
	this->MAX_MESSAGE_LENGTH = 400;
	this->error = false;

	if (command_parts.size() < 3)
	{
		this->error = true;
		this->errorMessage = "999 PRIVMSG :Invalid number of parameters.";
		return;
	}

	bool messageStarted = false;

	for (size_t i = 1; i < command_parts.size(); ++i)
	{
		if (command_parts[i][0] == ':' && !messageStarted)
		{
			this->message = command_parts[i].substr(1);
			messageStarted = true;
		}
		else if (messageStarted)
			this->message += " " + command_parts[i];
		else
			this->recipients.push_back(command_parts[i]);
	}

	if (this->message.length() > MAX_MESSAGE_LENGTH)
	{
		this->error = true;
		this->errorMessage = "999 PRIVMSG :Message too long (max " + std::to_string(MAX_MESSAGE_LENGTH) + " characters)";
		return;
	}

	if (this->message.empty())
	{
		this->error = true;
		this->errorMessage = "999 PRIVMSG :Message is missing.";
	}
}

void Commands::Privmsg::execute(Client& client, Server& server)
{	
	if (this->error)
	{
		client.sendBack(this->errorMessage, "client");
		return;
	}

	for (std::vector<std::string>::iterator it = recipients.begin(); it != recipients.end(); ++it)
	{
		std::string recipient = *it;
		if (recipient[0] == '#')
		{
			Channel* channel = server.getChannel(recipient);

			if (!channel)
			{
				client.sendBack("403 " + recipient + " :No such channel", "client");
				continue;
			}
			if (!channel->isMember(client))
			{
				client.sendBack("404 " + recipient + " :Cannot send to channel", "client");
				continue;
			}
			channel->sendBack("PRIVMSG " + recipient + " :" + message);
		}
		else
		{
			Client* target = server.getClientByNickname(recipient);

			if (!target)
			{
				client.sendBack("401 " + client.getNickname() + " " + recipient + " :No such nick/channel", "client");
				continue;
			}
			target->sendBack("PRIVMSG " + recipient + " :" + message, "client");
		}
	}
}
