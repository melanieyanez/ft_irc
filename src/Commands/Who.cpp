#include "Commands/Who.hpp"
#include "Client.hpp"
#include "Server.hpp"

Commands::Who::Who(std::vector<std::string> command_parts)
{
	if (command_parts.size() > 2)
	{
		this->error = true;
		this->errorMessage = "461 WHO :Too many parameters";
		return;
	}

	this->error = false;

	if (command_parts.size() == 1)
		this->Target = "";
	else
		this->Target = command_parts[1];
}
			
void Commands::Who::execute(Client& client, Server& server)
{
	if (this->error)
	{
		client.sendBack(this->errorMessage, "client");
		return;
	}

	else if (this->Target.empty())
		listConnectedUsers(server, client);
	else if (this->Target[0] == '#')
		listUsersInChannel(server, client, this->Target);
	else
		listSpecificUser(server, client, this->Target);
}

void Commands::Who::listConnectedUsers(Server &server, Client &client)
{
	std::vector<Client*> clients = server.getClients();
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		Client* connectedClient = *it;
		std::string response = "352 " + client.getNickname() + " * " + connectedClient->getNickname() + " * :" + connectedClient->getFullname();
		client.sendBack(response, "client");
	}
	std::string endResponse = "315 " + client.getNickname() +  " :End of /WHO list.";
	client.sendBack(endResponse, "client");
}
 			
void Commands::Who::listUsersInChannel(Server &server, Client &client, const std::string &channelName)
{
	Channel *channel = server.getChannel(channelName);

	if (channel)
	{
		std::vector<Client*> members = channel->getMembers();
		for (std::vector<Client*>::iterator it = members.begin(); it != members.end(); ++it)
		{
			Client* channelClient = *it;
			std::string response = "352 " + client.getNickname() + " " + channelName + " * " + channelClient->getNickname() + " * :" + channelClient->getFullname();
			client.sendBack(response, "client");
		}
		std::string endResponse = "315 " + client.getNickname() + " " + channelName + " :End of /WHO list.";
		client.sendBack(endResponse, "client");
	}
	else
	{
		std::string errorResponse = "403 " + client.getNickname() + " " + channelName + " :No such channel";
		client.sendBack(errorResponse, "client");
	}
}
			
void Commands::Who::listSpecificUser(Server &server, Client &client, const std::string &nickName)
{
	Client *specificClient = server.getClientByNickname(nickName);

	if (specificClient)
	{
		std::string response = "352 " + client.getNickname() + " * " + specificClient->getNickname() + " * :" + specificClient->getFullname();
		client.sendBack(response, "client");
	}
	std::string endResponse = "315 " + client.getNickname() +  " :End of /WHO list.";
	client.sendBack(endResponse, "client");
}