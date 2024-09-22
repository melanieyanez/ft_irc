#include "Commands/Topic.hpp"


Commands::Topic::Topic(std::vector<std::string> command_parts)
{
	this->error = false;
	if (command_parts.size() < 2 || command_parts.size() > 3)
	{
		this->error = true;
		this->errorMessage = "461 MODE :Not enough parameters.";
		return;
	}
	
	this->channel = command_parts[1];

	// check si il y a un topic
	isSettingTopic = (command_parts.size() == 3);
	if (isSettingTopic)
		this->topic = command_parts[2];
}

void Commands::Topic::execute(Client& client, Server& server)
{
	if (this->error)
	{
		client.sendBack(this->errorMessage, "client");
		return;
	}

	// recuperer le channel depuis le server
	Channel* channel = server.getChannel(this->channel);

	// si le channel n'existe pas
	if (!channel)
	{
		client.sendMessage("403 " + this->channel + " :No such channel");
		return;
	}

	if (isSettingTopic)
		setTopic(client, server, *channel);
	else
		getCurrentTopic(client, server, *channel);
}

void Commands::Topic::setTopic(Client& client, Server&, Channel& channel)
{

	if (channel.isTopicRestricted() && !channel.isOperator(client))
	{
		client.sendBack("482 " + channel.getChannelName() + " :You're not channel operator", "client");
		return;
	}
	// set le topic dans le channel
	channel.setTopic(this->topic);
	channel.setLastTopicSetter(client.getNickname());
	channel.setLastTopicSetTime();

	// envois du topic a tout les membre du channel
	channel.sendBack("TOPIC " + channel.getChannelName() + " :" + this->topic);
}

void Commands::Topic::getCurrentTopic(Client& client, Server&, Channel& channel)
{
	if (channel.getTopic().empty())
		client.sendBack("331 " + client.getNickname() + " " + channel.getChannelName() + " :No topic is set", "client");
	else
	{
		client.sendBack("332 " + client.getNickname() + " " + channel.getChannelName() + " :" + channel.getTopic(), "client");
		client.sendBack("333 " + client.getNickname() + " " + channel.getChannelName() + " " + channel.getLastTopicSetter() + " " + channel.getLastTopicSetTime(), "client");
	}
}
