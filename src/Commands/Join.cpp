#include "Commands/Join.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Reply.hpp"

#include <sstream>
#include <iostream>
#include <map>

Commands::Join::Join(std::vector<std::string> command_parts)
{
	this->error = false;

	if (command_parts.size() < 2)
	{
		this->error = true;
		this->errorCode = 461;
		return;
	}

	std::stringstream channelStream(command_parts[1]);
	std::string channel;
	while (std::getline(channelStream, channel, ','))
		this->channels.push_back(channel);

	if (command_parts.size() >= 3)
	{
		std::stringstream keyStream(command_parts[2]);
		std::string key;
		while (std::getline(keyStream, key, ','))
			this->keys.push_back(key);
	}

	while (this->keys.size() < this->channels.size())
		this->keys.push_back("");
}

void Commands::Join::execute(Client& client, Server& server)
{
	Reply reply;

	client.sendMessage("Executing JOIN command for client: " + client.getNickname(), "console");

	if (this->error)
	{
		reply.sendReply(this->errorCode, client, NULL, NULL, &server, "JOIN");
		return;
	}

	std::map<std::string, std::string> channelKeyMap;

	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string channelName = channels[i];
		std::string key = keys[i];

		Channel* channel = server.getChannel(channelName);

		if (channel && channel->isProtected())
			channelKeyMap[channelName] = key;
		else
			channelKeyMap[channelName] = "";
	}

	for (std::map<std::string, std::string>::iterator it = channelKeyMap.begin(); it != channelKeyMap.end(); ++it)
	{
		std::string channelName = it->first;
		std::string key = it->second;

		client.sendMessage("Attempting to join channel: " + channelName, "console");

		Channel* channel = server.getChannel(channelName);
		if (!channel)
		{
			client.sendMessage( "Channel not found, creating new channel: " + channelName, "console");
			channel = new Channel(channelName, "");
			server.addChannel(channel);
		}

		if (channel->isProtected() && !channel->isCorrectKey(key))
		{
			reply.sendReply(475, client, NULL, channel, &server, "JOIN");
			continue;
		}

		if (channel->isMember(client))
		{
			reply.sendReply(443, client, NULL, channel, &server, "JOIN", client.getNickname());
			continue;
		}

		if (channel->isInvitationOnly() && !channel->isMember(client) && !channel->isInvited(client))
		{
			reply.sendReply(473, client, NULL, channel, &server, "JOIN");
			continue;
		}

		if (!channel->addMember(client))
		{
			reply.sendReply(471, client, NULL, channel, &server, "JOIN");
			continue;
		}

		channel->sendMessage(":" + client.getFullIdentifier() + " JOIN :" + channel->getChannelName());
		reply.sendReply(353, client, NULL, channel, &server, "JOIN");
		reply.sendReply(366, client, NULL, channel, &server, "JOIN");

		client.sendMessage("Client " + client.getNickname() + " successfully joined channel: " + channelName, "console");
	}
}


/*
void Commands::Join::execute(Client& client, Server& server)
{
	ANCIEN TEST
	
	
	// Je stock le nom du channel dans le nouvel objet channel
	Channel* channel = server.getChannel(this->channelName);

	// si il n'existe pas, je malloc un nouveau channel qui va prendre le nom de ce que j'ai mis dans command_parts
	if (!channel)
	{
		channel = new Channel(this->channelName);
		server.addChannel(channel);
	}

	// controle si le client n'est pas deja dans le channel
	if (channel->isMember(client))
	{
		client.sendMessage("You are already in the channel : " + channelName);
		return;
	}

	// ajouter le client au channel

	channel->addMember(&client);
	server.addChannel(channel);

	// envoyer la notification a pour le join
	client.sendMessage(":" + client.getNickname() + " JOIN " + channelName);

	// envoyer la notif a tout les autre membres du channel
	std::vector<Client*> members = channel->getMembers();
	for (std::vector<Client *>::iterator it = members.begin(); it != members.end(); ++it)
	{
		if ((*it)->getNickname() != client.getNickname())
			(*it)->sendMessage(":" + client.getNickname() + " JOIN " + channelName);
	}	
}
*/