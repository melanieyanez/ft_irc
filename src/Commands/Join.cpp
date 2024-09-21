#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Commands/Join.hpp"
#include <sstream>
#include <iostream>

Commands::Join::Join(std::vector<std::string> command_parts)
{
	if (command_parts.size() < 2)
		throw ;

	std::stringstream channelStream(command_parts[1]);
	std::string channel;

	// check si il y a plusieurs channels separes par une virgule
	std::cout << "Commands::Join::Join: channels in param: " << command_parts[1] << std::endl;
	while (std::getline(channelStream, channel, ','))
	{
		std::cout << "Commands::Join::Join: adding one more channel: " << channel << std::endl;
		this->channels.push_back(channel);
	}

	// si les keys (password des channel) sont presentes dans la commande
	if (command_parts.size() >= 3)
	{
		std::stringstream ss(command_parts[2]);
		std::string item;

		//check si il y a plusieurs keys separes par une virgule
		while (std::getline(ss, item, ','))
			this->keys.push_back(item);
	}
}

void Commands::Join::execute(Client& client, Server& server)
{
	// size_t channelMax = 10;

	std::cout << "Executing JOIN command for client: " << client.getNickname() << std::endl;

	// check si nombre maximum de channels atteint, si oui, on envoie un message d'erreur

	// pour chaque channel, on cree un channel si il n'existe pas, on ajoute le client au channel et on envoie les notifs
	std::cout << "Commands::Join::execute : " << channels.size() << std::endl;
	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string channelName = channels[i];
		std::string key = (i < keys.size()) ? keys[i] : "";

		std::cout << "Attempting to join channel: " << channelName << std::endl;

		// cree un channel si il n'existe pas et l'ajoute au serveur
		Channel* channel = server.getChannel(channelName);
		if (!channel)
		{
			std::cout << "Channel not found, creating new channel: " << channelName << std::endl;
			channel = new Channel(channelName, key);
			server.addChannel(channel);
		}
		if (!key.empty() && !channel->isCorrectKey(key))
		{
			client.sendBack("Wrong key for channel: " + channelName);
			continue;
		}

		/*
		if (check si channel priver && !channel->isMember(client) && !channel->isInvited(client))
		{
			client.sendMessage("You are not invited to join the channel : " + channelName);
			return;
		}
		*/

		// si le client est deja dans le channel, on envoie un message d'erreur
		if (channel->isMember(client))
		{
			client.sendMessage("You are already in the channel : " + channelName);
			continue;
		}

		std::cout << "addMember to " << channelName << " with client: " << client.getFullIdentifier() << std::endl;
		if (!channel->addMember(client))
		{
			client.sendBack("Channel is full");
			continue;
		}


		client.sendBack(client.getFullIdentifier() + " JOIN " + channelName);

		std::vector<Client*> members = channel->getMembers();

		// envoie la notif a tout les autres membres du channel que quelqu'un a rejoint
		for (std::vector<Client *>::iterator it = members.begin(); it != members.end(); ++it)
		{
			if ((*it)->getNickname() != client.getNickname())
				(*it)->sendBack(client.getFullIdentifier() + " JOIN " + channelName);
		}
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