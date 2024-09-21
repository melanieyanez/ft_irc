#include "Commands/Topic.hpp"


Commands::Topic::Topic(std::vector<std::string> command_parts)
{
	if (command_parts.size() < 2)
		throw ;
	this->channel = command_parts[1];

	// check si il y a un topic
	isSettingTopic = command_parts.size() == 3;
	if (command_parts.size() > 2)
		this->topicMessage = command_parts[2];
}

void Commands::Topic::execute(Client& client, Server& server)
{
	// recuperer le channel depuis le server
	Channel* channel = server.getChannel(this->channel);

	// si le channel n'existe pas
	if (!channel)
	{
		client.sendMessage("Channel : " + this->channel + " does not exist");
		return;
	}

	if (isSettingTopic)
		setTopic(client, server, *channel);
	else
		getCurrentTopic(client, server, *channel);
}

void Commands::Topic::setTopic(Client& client, Server&, Channel& channel)
{

	// set le topic dans le channel
	channel.setTopic(this->topicMessage, client);

	// envois du topic a tout les membre du channel
	std::vector<Client*> members = channel.getMembers();
	for (std::vector<Client*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		(*it)->sendBack("TOPIC " + channel.getChannelName() + " :" + this->topicMessage);
	}
}

void Commands::Topic::getCurrentTopic(Client& client, Server&, Channel& channel)
{
	client.sendBack("TOPIC " + channel.getChannelName() + " :" + channel.getTopic());
}
