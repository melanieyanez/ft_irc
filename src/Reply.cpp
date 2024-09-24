#include "Reply.hpp"

void Reply::sendReply(int code, Client &client, Client *target, Channel *channel, Server *server, const std::string command, std::string extra)
{
	(void)server;
	switch (code)
	{
		case 001: //USED
			rplWelcome(client);
			break;
		case 303: //USED
			rplIson(client, extra);
			break;
		case 315:
			rplEndOfWho(client);
			break;
		case 322: //USED
			if (channel) 
				rplList(client, *channel);
			break;
		case 323: //USED
			rplListEnd(client);
			break;
		case 331:
			if (channel)
				rplNoTopic(client, *channel);
			break;
		case 332:
			if (channel)
				rplTopic(client, *channel);
			break;
		case 333:
			if (channel)
				rplTopicWhoTime(*channel, client);
			break;
		case 341: //USED
			if (target && channel)
				rplInviting(client, *channel, *target);
			break;
		case 352:
			if (channel)
				rplWhoReply(*channel, client);
			break;
		case 353: //USED
			if (channel)
				rplNamReply(client, *channel);
			break;
		case 366: //USED
			if (channel)
				rplEndOfNames(client, *channel);
			break;
		case 401: //USED
			errNoSuchNick(client, extra);
			break;
		case 403: //USED
			errNoSuchChannel(client, extra);
			break;
		case 404:
			if (channel)
				errCannotSendToChan(*channel, client);
			break;
		case 431: //USED
			errNoNickNameGiven(client, command);
			break;
		case 432:
			errErroneousNickName(client);
			break;
		case 433:
			errNickNameInUse(client);
			break;
		case 441://USED
			if (channel)
				errUserNotInChannel(client, *channel, *target);
			break;
		case 442: //USED
			if (channel)
				errNotOnChannel(client, *channel);
			break;
		case 443: //USED
			if (channel)
				errUserOnChannel(client, *channel, extra);
			break;
		case 451: //USED
			errNotRegistered(client, command);
			break;
		case 461: //USED
			errNeedMoreParams(client, command);
			break;
		case 462: //USED
			errAlreadyRegistered(client, command);
			break;
		case 464:
			errPasswordMismatch(client);
			break;
		case 471: //USED
			if (channel)
				errChannelIsFull(client, *channel);
			break;
		case 473: //USED
			if (channel)
				errInviteOnlyChan(client, *channel);
			break;
		case 475: //USED
			if (channel)
				errBadChannelKey(client, *channel);
			break;
		case 482: //USED
			if (channel)
				errChanOpPrivsNeeded(*channel, client);
			break;
		case 704: //USED
			rplHelpStart(client, command);
			break;
		case 705: //USED
			rplEndOfHelp(client, command);
			break;
		case 999: //USED
			errUnknownCommand(client, command);
			break;
		default:
			client.sendBack("Unknown reply code: " + std::to_string(code));
	}
}


void Reply::rplWelcome(Client &client)
{
	client.sendBack("001 " + client.getNickname() + " :Welcome to the Internet Relay Network " + client.getNickname() + "!", "client");
}

void Reply::rplIson(Client &client, std::string extra)
{
	client.sendBack("303 " + client.getNickname() + " :" + extra, "client");
}

void Reply::rplEndOfWho(Client &client)
{
	client.sendBack("315 " + client.getNickname() + " :End of /WHO list.");
}

void Reply::rplList(Client &client, Channel &channel)
{
	std::string topic = channel.getTopic().empty() ? "No topic is set" : channel.getTopic();
	client.sendMessage("Listing channel: " + channel.getChannelName() + " - No topic is set - Members: " + std::to_string(channel.getMemberCount()), "console");
	client.sendBack("322 " + client.getNickname() + " " + channel.getChannelName() + " " + std::to_string(channel.getMemberCount()) + " :" + topic, "client");
}

void Reply::rplListEnd(Client &client)
{
	client.sendMessage("End of /LIST command for client: " + client.getNickname(), "console");
	client.sendBack("323 " + client.getNickname() + " :End of /LIST", "client");
}

void Reply::rplNoTopic(Client &client, Channel &channel)
{
	client.sendBack("331 * " + channel.getChannelName() + " :No topic is set");
}

void Reply::rplTopic(Client &client, Channel &channel)
{
	client.sendBack("332 * " + channel.getChannelName() + " :" + channel.getTopic());
}

void Reply::rplTopicWhoTime(Channel &channel, Client &client)
{
	client.sendBack("333 * " + channel.getChannelName() + " " + channel.getLastTopicSetter() + " " + channel.getLastTopicSetTime());
}

void Reply::rplInviting(Client &client, Channel &channel, Client &target)
{
	target.sendMessage(client.getFullIdentifier() + " INVITE " + target.getNickname() + " :" + channel.getChannelName());
	client.sendBack("341 " + client.getNickname() + " " + target.getNickname() + " :" + channel.getChannelName(), "client");
}

void Reply::rplWhoReply(Channel &channel, Client &client)
{
	std::string userInfo = client.getFullIdentifier();
	client.sendBack("352 * " + channel.getChannelName() + " " + userInfo + " :H");
}

void Reply::rplNamReply(Client &client, Channel &channel)
{
	client.sendBack("353 " + client.getNickname() + " = " + channel.getChannelName() + " :" + channel.getMemberList(), "client");
}

void Reply::rplEndOfNames(Client &client, Channel &channel)
{
	client.sendBack("366 " + client.getNickname() + " " + channel.getChannelName() + " :End of /NAMES list.", "client");
}

void Reply::errNoSuchNick(Client &client, std::string extra)
{
	client.sendBack("401 " + client.getNickname() + " " + extra + " :No such nick/channel");
}

void Reply::errNoSuchChannel(Client &client, std::string extra)
{
	client.sendBack("403 " + client.getNickname() + " " + extra + " :No such channel", "client");
}

void Reply::errCannotSendToChan(Channel &channel, Client &client)
{
	client.sendBack("404 * " + channel.getChannelName() + " :Cannot send to channel");
}

void Reply::errNoNickNameGiven(Client &client, std::string command)
{
	client.sendBack("431 " + client.getNickname() + " " + command + " :No nickname given", "client");
}

void Reply::errErroneousNickName(Client &client)
{
	client.sendBack("432 " + client.getNickname() + " :Erroneous nickname");
}

void Reply::errNickNameInUse(Client &client)
{
	client.sendBack("433 " + client.getNickname() + " :Nickname is already in use");
}

void Reply::errUserNotInChannel(Client &client, Channel &channel, Client &target)
{
	client.sendBack("441 " + client.getNickname() + " " + target.getNickname() + " " + channel.getChannelName() + " :They aren't on that channel", "client");
}

void Reply::errNotOnChannel(Client &client, Channel &channel)
{
	client.sendBack("442 " + client.getNickname() + " " + channel.getChannelName() + " :You're not on that channel");
}

void Reply::errUserOnChannel(Client &client, Channel &channel, std::string extra)
{
	client.sendBack("443 " + client.getNickname() + " " + extra + " " + channel.getChannelName() + " :is already on channel", "client");
}

void Reply::errNotRegistered(Client &client, std::string command)
{
	client.sendMessage("Error 451: " + command + " - Client " + client.getNickname() + " is not registered", "console");
	client.sendBack("451 " + client.getNickname() + " " + command + " :You have not registered", "client");
}

void Reply::errNeedMoreParams(Client &client, std::string command)
{
	client.sendMessage("Error 461: " + command + " - Wrong number of parameters for client : " + client.getNickname(), "console");
	client.sendBack("461 " + client.getNickname() + " " + command + " :Wrong number of parameters", "client");
}

void Reply::errAlreadyRegistered(Client &client, std::string command)
{
	client.sendBack("462 " + client.getNickname() + " " + command + " :You may not reregister", "client");
}

void Reply::errPasswordMismatch(Client &client)
{
	client.sendBack("464 " + client.getNickname() + " :Password incorrect");
}

void Reply::errChannelIsFull(Client &client, Channel &channel)
{
	client.sendBack("471 " + client.getNickname() + " " + channel.getChannelName() + " :Cannot join channel (+l) - Channel is full.", "client");
}

void Reply::errInviteOnlyChan(Client &client, Channel &channel)
{
	client.sendBack("473 " + client.getNickname() + " " + channel.getChannelName() + " :Cannot join channel (+i) - You are not invited.", "client");
}

void Reply::errBadChannelKey(Client &client, Channel &channel)
{
	client.sendBack("475 " + client.getNickname() + " " + channel.getChannelName() + " :Cannot join channel (+k) - Wrong key.", "client");
}

void Reply::errChanOpPrivsNeeded(Channel &channel, Client &client)
{
	client.sendBack("482 " + client.getNickname() + " " + channel.getChannelName() + " :You're not channel operator", "client");
}

void Reply::rplHelpStart(Client &client, const std::string command)
{
	client.sendBack("704 " + client.getNickname() + " " + command + " :Start of help message", "client");
}

void Reply::rplEndOfHelp(Client &client, const std::string command)
{
	client.sendBack("705 " + client.getNickname() + " " + command + " :End of help message", "client");
}

void Reply::errUnknownCommand(Client &client, const std::string command)
{
	client.sendBack("999 " + client.getNickname() + " " + command + " :Invalid format", "client");
}

