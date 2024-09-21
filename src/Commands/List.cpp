#include "Commands/List.hpp"
#include "Server.hpp"
#include "Client.hpp"

Commands::List::List(std::vector<std::string> command_parts)
{
	this->Param = false;
	if (command_parts.size() > 1)
		this->Param = true;
}
		
void Commands::List::execute(Client& client, Server& server)
{
	if (this->Param)
	{
		std::string errorResponse = "461 " + client.getNickname() + " LIST :Too many parameters";
		client.sendBack(errorResponse, "client");
		return ;
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