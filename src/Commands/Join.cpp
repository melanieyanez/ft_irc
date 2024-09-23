#include "Commands/Join.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

#include <sstream>
#include <iostream>
#include <map>

Commands::Join::Join(std::vector<std::string> command_parts)
{
	this->error = false;

	if (command_parts.size() < 2)
	{
		this->error = true;
		this->errorMessage = "999 JOIN :Invalid number of parameters.";
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
	if (this->error)
	{
		client.sendBack(this->errorMessage, "client");
		return;
	}

	std::cout << "Executing JOIN command for client: " << client.getNickname() << std::endl;

	std::map<std::string, std::string> channelKeyMap;

	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string channelName = channels[i];
		std::string key = keys[i];

		Channel* channel = server.getChannel(channelName);

		if (channel && channel->isProtected())
			channelKeyMap[channelName] = key;
		
		else
			channelKeyMap[channelName] = " ";
	}

	for (std::map<std::string, std::string>::iterator it = channelKeyMap.begin(); it != channelKeyMap.end(); ++it)
	{
		std::string channelName = it->first;
		std::string key = it->second;

		std::cout << "Attempting to join channel: " << channelName << " with key: " << key << std::endl;

		Channel* channel = server.getChannel(channelName);
		if (!channel)
		{
			std::cout << "Channel not found, creating new channel: " << channelName << std::endl;
			channel = new Channel(channelName, "");
			server.addChannel(channel);
		}

		if (channel->isProtected() && !channel->isCorrectKey(key))
		{
			client.sendBack("475 " + channelName + " :Cannot join channel (+k) - Wrong key.");
			continue;
		}

		if (channel->isMember(client))
		{
			client.sendBack("443 " + client.getNickname() + " " + channelName + " :You are already in the channel");
			continue;
		}

		if (channel->isInvitationOnly() && !channel->isMember(client) && !channel->isInvited(client))
		{
			client.sendBack("473 " + channelName + " :Cannot join channel (+i) - You are not invited.");
			continue;
		}

		if (!channel->addMember(client))
		{
			client.sendBack("471 " + channelName + " :Cannot join channel (+l) - Channel is full.");
			continue;
		}

		client.sendBack(client.getFullIdentifier() + " JOIN " + channelName);
		channel->sendBack(client.getFullIdentifier() + " JOIN " + channelName);		

		std::string memberList = channel->getMemberList();
		client.sendBack("353 " + client.getNickname() + " = " + channelName + " :" + memberList, "client");

		client.sendBack("366 " + client.getNickname() + " " + channelName + " :End of /NAMES list", "client");

		std::cout << "Client " << client.getNickname() << " successfully joined channel: " << channelName << std::endl;
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