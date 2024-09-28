#include "Reply.hpp"

void Reply::sendReply(int code, Client &client, Client *target, Channel *channel, Server *server, const std::string command, std::string extra)
{
	(void)server;
	switch (code)
	{
		case 001:
			rplWelcome(client);
			break;
		case 303:
			rplIson(client, extra);
			break;
		case 315:
			rplEndOfWho(client);
			break;
		case 322:
			if (channel) 
				rplList(client, *channel);
			break;
		case 323:
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
		case 341:
			if (target && channel)
				rplInviting(client, *channel, *target);
			break;
		case 352:
			rplWhoReply(channel, client, target, extra);
			break;
		case 353:
			if (channel)
				rplNamReply(client, *channel);
			break;
		case 366:
			if (channel)
				rplEndOfNames(client, *channel);
			break;
		case 401:
			errNoSuchNick(client, extra, command);
			break;
		case 403:
			errNoSuchChannel(client, command, extra);
			break;
		case 404:
			if (channel)
				errCannotSendToChan(*channel, client, command);
			break;
		case 431:
			errNoNickNameGiven(client, command);
			break;
		case 432:
			errErroneousNickName(client, command);
			break;
		case 433:
			errNickNameInUse(client, command);
			break;
		case 441:
			if (channel && target)
				errUserNotInChannel(client, *channel, *target, command);
			break;
		case 442:
			if (channel)
				errNotOnChannel(client, *channel, command);
			break;
		case 443:
			if (channel)
				errUserOnChannel(client, *channel, extra, command);
			break;
		case 451:
			errNotRegistered(client, command);
			break;
		case 461:
			errNeedMoreParams(client, command);
			break;
		case 462:
			errAlreadyRegistered(client, command);
			break;
		case 464:
			errPasswordMismatch(client, command);
			break;
		case 467:
			if (channel)
				errChannelKeyAlreadySet(client, *channel);
			break;
		case 471:
			if (channel)
				errChannelIsFull(client, *channel, command);
			break;
		case 472:
			if (channel)
				errInvalidMode(client, *channel, command);
			break;
		case 473:
			if (channel)
				errInviteOnlyChan(client, *channel, command);
			break;
		case 475:
			if (channel)
				errBadChannelKey(client, *channel, command);
			break;
		case 476:
				errBadChannelMask(client, extra);
			break;
		case 482:
			if (channel)
				errChanOpPrivsNeeded(*channel, client, command);
			break;
		case 704:
			rplHelpStart(client, command);
			break;
		case 705:
			rplEndOfHelp(client, command);
			break;
		case 999:
			errUnknownCommand(client, command);
			break;
		default:
			client.sendBack("Unknown reply code: " + std::to_string(code));
	}
}

void Reply::rplWelcome(Client &client)
{
	client.sendMessage("New client connected: " + client.getFullIdentifier(), "console");
	client.sendBack("001 " + client.getNickname() + " :Welcome to the Internet Relay Network " + client.getNickname() + "!", "client");
}

void Reply::rplIson(Client &client, std::string extra)
{
	client.sendMessage("ISON response for client: " + client.getNickname(), "console");
	client.sendBack("303 " + client.getNickname() + " :" + extra, "client");
}

void Reply::rplEndOfWho(Client &client)
{
	client.sendMessage("End of /WHO list for client: " + client.getNickname(), "console");
	client.sendBack("315 " + client.getNickname() + " :End of /WHO list.", "client");
}

void Reply::rplList(Client &client, Channel &channel)
{
	std::string topic = channel.getTopic().empty() ? "No topic is set" : channel.getTopic();
	client.sendMessage("Listing channel: " + channel.getChannelName() + " - Topic: " + topic + " - Members: " + std::to_string(channel.getMemberCount()), "console");
	client.sendBack("322 " + client.getNickname() + " " + channel.getChannelName() + " " + std::to_string(channel.getMemberCount()) + " :" + topic, "client");
}

void Reply::rplListEnd(Client &client)
{
	client.sendMessage("End of /LIST command for client: " + client.getNickname(), "console");
	client.sendBack("323 " + client.getNickname() + " :End of /LIST", "client");
}

void Reply::rplNoTopic(Client &client, Channel &channel)
{
	client.sendMessage("No topic is set for channel: " + channel.getChannelName(), "console");
	client.sendBack("331 " + client.getNickname() + " " + channel.getChannelName() + " :No topic is set", "client");
}

void Reply::rplTopic(Client &client, Channel &channel)
{
	client.sendMessage("Displaying topic for channel: " + channel.getChannelName(), "console");
	client.sendBack("332 " + client.getNickname() + " " + channel.getChannelName() + " :" + channel.getTopic(), "client");
}

void Reply::rplTopicWhoTime(Channel &channel, Client &client)
{
	client.sendMessage("Topic last changed in channel: " + channel.getChannelName() + " by " + channel.getLastTopicSetter(), "console");
	client.sendBack("333 " + client.getNickname() + " " + channel.getChannelName() + " " + channel.getLastTopicSetter() + " " + channel.getLastTopicSetTime(), "client");
}

void Reply::rplInviting(Client &client, Channel &channel, Client &target)
{
	client.sendMessage("Success: Client " + client.getNickname() + " invited " + target.getNickname() + " to channel " + channel.getChannelName(), "console");
	target.sendMessage(":" + client.getFullIdentifier() + " INVITE " + target.getNickname() + " :" + channel.getChannelName());
	client.sendBack("341 " + client.getNickname() + " " + target.getNickname() + " :" + channel.getChannelName(), "client");
}

void Reply::rplWhoReply(Channel *channel, Client &client, Client *target, std::string extra)
{
 	std::string response;
	
	if (extra == "channel" && target && channel)
		response = "352 " + client.getNickname() + " " + channel->getChannelName() + " " + target->getNickname() + " :" + target->getFullname();
	else if (extra == "user" && target)
		response = "352 " + client.getNickname() + " * " + target->getNickname() + " :" + target->getFullname();
	else if (extra == "server" && target)
		response = "352 " + client.getNickname() + " * " + target->getNickname() + " :" + target->getFullname();
	else
	{
		client.sendMessage("WHO reply error - Invalid parameters", "console");
		return;
	}
	client.sendBack(response, "client");
}

void Reply::rplNamReply(Client &client, Channel &channel)
{
	client.sendMessage("Listing members of channel: " + channel.getChannelName(), "console");
	client.sendBack("353 " + client.getNickname() + " = " + channel.getChannelName() + " :" + channel.getMemberList(), "client");
}

void Reply::rplEndOfNames(Client &client, Channel &channel)
{
	client.sendMessage("End of /NAMES list for channel: " + channel.getChannelName(), "console");
	client.sendBack("366 " + client.getNickname() + " " + channel.getChannelName() + " :End of /NAMES list.", "client");
}

void Reply::errNoSuchNick(Client &client, std::string extra, std::string command)
{
	client.sendMessage("Error 401: " + command + " - Client " + extra + " does not exist or is not connected", "console");
	client.sendBack("401 " + client.getNickname() + " " + extra + " :No such nick/channel", "client");
}

void Reply::errNoSuchChannel(Client &client, std::string command, std::string extra)
{
	client.sendMessage("Error 403: " + command + " - Channel " + extra + " - No such channel", "console");
	client.sendBack("403 " + client.getNickname() + " " + extra + " :No such channel", "client");
}

void Reply::errCannotSendToChan(Channel &channel, Client &client, std::string command)
{
	client.sendMessage("Error 404: " + command + " - Cannot send to channel: " + channel.getChannelName(), "console");
	client.sendBack("404 " + client.getNickname() + " " + channel.getChannelName() + " :Cannot send to channel", "client");
}

void Reply::errNoNickNameGiven(Client &client, std::string command)
{
	client.sendMessage("Error 431: " + command + " - No nickname given", "console");
	client.sendBack("431 " + client.getNickname() + " :No nickname given", "client");
}

void Reply::errErroneousNickName(Client &client, std::string command)
{
	client.sendMessage("Error 432: " + command + " - Erroneous nickname", "console");
	client.sendBack("432 " + client.getNickname() + " :Erroneous nickname", "client");
}

void Reply::errNickNameInUse(Client &client, std::string command)
{
	client.sendMessage("Error 433: " + command + " - Nickname is already in use", "console");
	client.sendBack("433 " + client.getNickname() + " :Nickname is already in use", "client");
}

void Reply::errUserNotInChannel(Client &client, Channel &channel, Client &target, std::string command)
{
	client.sendMessage("Error 441: " + command + " - Client " + target.getNickname() + " is not in channel " + channel.getChannelName(), "console");
	client.sendBack("441 " + client.getNickname() + " " + target.getNickname() + " " + channel.getChannelName() + " :They aren't on that channel", "client");
}

void Reply::errNotOnChannel(Client &client, Channel &channel, std::string command)
{
	client.sendMessage("Error 442: " + command + " - Client " + client.getNickname() + " is not a member of channel " + channel.getChannelName(), "console");
	client.sendBack("442 " + client.getNickname() + " " + channel.getChannelName() + " :You're not on that channel", "client");
}

void Reply::errUserOnChannel(Client &client, Channel &channel, std::string extra, std::string command)
{
	client.sendMessage("Error 443: " + command + " - Client " + extra + " is already in channel " + channel.getChannelName(), "console");
	client.sendBack("443 " + client.getNickname() + " " + extra + " " + channel.getChannelName() + " :is already on channel", "client");
}

void Reply::errNotRegistered(Client &client, std::string command)
{
	client.sendMessage("Error 451: " + command + " - Client " + client.getNickname() + " is not registered", "console");
	client.sendBack("451 " + client.getNickname() + " " + command + " :You have not registered", "client");
}

void Reply::errNeedMoreParams(Client &client, std::string command)
{
	client.sendMessage("Error 461: " + command + " - Wrong number of parameters for client: " + client.getNickname(), "console");
	client.sendBack("461 " + client.getNickname() + " " + command + " :Wrong number of parameters", "client");
}

void Reply::errAlreadyRegistered(Client &client, std::string command)
{
	client.sendMessage("Error 462: " + command + " - Client " + client.getNickname() + " is already registered", "console");
	client.sendBack("462 " + client.getNickname() + " " + command + " :You may not reregister", "client");
}

void Reply::errPasswordMismatch(Client &client, std::string command)
{
	client.sendMessage("Error 464: " + command + " - Password mismatch for client: " + client.getNickname(), "console");
	client.sendBack("464 " + client.getNickname() + " :Password incorrect", "client");
}

void Reply::errChannelKeyAlreadySet(Client &client, Channel &channel)
{
	client.sendMessage("Error 467: Channel key already set for channel " + channel.getChannelName(), "console");
	client.sendBack("467 " + client.getNickname() + " " + channel.getChannelName() + " :Channel key already set", "client");
}

void Reply::errChannelIsFull(Client &client, Channel &channel, std::string command)
{
	client.sendMessage("Error 471: " + command + " - Channel " + channel.getChannelName() + " is full", "console");
	client.sendBack("471 " + client.getNickname() + " " + channel.getChannelName() + " :Cannot join channel (+l) - Channel is full", "client");
}

void Reply::errInvalidMode(Client &client, Channel &channel, const std::string command)
{
	client.sendMessage("Error 472: Unknown mode " + command + " on channel " + channel.getChannelName(), "console");
	client.sendBack("472 " + client.getNickname() + " " + command + " :Unknown mode flag", "client");
}

void Reply::errInviteOnlyChan(Client &client, Channel &channel, std::string command)
{
	client.sendMessage("Error 473: " + command + " - Channel " + channel.getChannelName() + " is invite-only", "console");
	client.sendBack("473 " + client.getNickname() + " " + channel.getChannelName() + " :Cannot join channel (+i) - You are not invited", "client");
}

void Reply::errBadChannelKey(Client &client, Channel &channel, std::string command)
{
	client.sendMessage("Error 475: " + command + " - Wrong key for channel " + channel.getChannelName(), "console");
	client.sendBack("475 " + client.getNickname() + " " + channel.getChannelName() + " :Cannot join channel (+k) - Wrong key", "client");
}

void Reply::errBadChannelMask(Client &client, const std::string &channel)
{
	client.sendMessage("Error 476: Invalid channel name " + channel, "console");
	client.sendBack("476 " + client.getNickname() + " " + channel + " :Invalid channel name", "client");
}

void Reply::errChanOpPrivsNeeded(Channel &channel, Client &client, std::string command)
{
	client.sendMessage("Error 482: " + command + " - Client " + client.getNickname() + " does not have operator privileges in channel " + channel.getChannelName(), "console");
	client.sendBack("482 " + client.getNickname() + " " + channel.getChannelName() + " :You're not channel operator", "client");
}

void Reply::rplHelpStart(Client &client, const std::string command)
{
	client.sendMessage("Start of help message for command: " + command, "console");
	client.sendBack("704 " + client.getNickname() + " " + command + " :Start of help message", "client");
}

void Reply::rplEndOfHelp(Client &client, const std::string command)
{
	client.sendMessage("End of help message for command: " + command, "console");
	client.sendBack("705 " + client.getNickname() + " " + command + " :End of help message", "client");
}

void Reply::errUnknownCommand(Client &client, const std::string command)
{
	client.sendMessage("Error 999: " + command + " - Unknown command", "console");
	client.sendBack("999 " + client.getNickname() + " " + command + " :Unknown command", "client");
}