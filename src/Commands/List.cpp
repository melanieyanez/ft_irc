#include "Commands/List.hpp"
#include "Server.hpp"
#include "Client.hpp"

Commands::List::List(std::vector<std::string> command_parts)
{
	if (command_parts.size() > 1)
	{
		this->error = true;
		this->errorMessage = "461 LIST :Too many parameters";
		return;
	}
	this->error = false;
}
		
void Commands::List::execute(Client& client, Server& server)
{
	 if (this->error)
	 {
		client.sendBack(this->errorMessage, "client");
		return;
	}

	std::vector<Channel*> channels = server.getChannels();
	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel* channel = *it;

		std::string response = "322 " + client.getNickname() + " " + channel->getChannelName() + " " + std::to_string(channel->getMemberCount()) + " :" + (channel->getTopic().empty() ? "No topic set" : channel->getTopic());
		client.sendBack(response, "client");
	}

	std::string endResponse = "323 " + client.getNickname() + " :End of /LIST";
	client.sendBack(endResponse, "client");
}