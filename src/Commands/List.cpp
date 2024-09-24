#include "Commands/List.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Reply.hpp"

#include <iostream>

Commands::List::List(std::vector<std::string> command_parts)
{
	if (command_parts.size() > 1)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}
	this->error = false;
}
		
void Commands::List::execute(Client& client, Server& server)
{
	Reply reply;

	client.sendMessage("Executing LIST command for client: " + client.getNickname(), "console");

	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, &server, "LIST");
		return;
	}

	std::vector<Channel*> channels = server.getChannels();
	
	if (channels.empty())
		client.sendMessage("No channels available to list.", "console");

	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel* channel = *it;

		reply.sendReply(322, client, NULL, channel, &server, "LIST");
	}

	reply.sendReply(323, client, NULL, NULL, &server, "LIST");
}