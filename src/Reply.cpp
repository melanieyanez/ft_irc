#include "Reply.hpp"

void Reply::sendReply(int code, Client &client, Client *target, Channel *channel, Server *server, const std::string command, std::string extra)
{
	switch (code)
	{
		case 001:
			rplWelcome(client);
			break;
		case 303:
			rplIson(client, *server);
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
                rplInviting(client, *channel, *target, *server);
            break;
		case 352:
			if (channel)
				rplWhoReply(*channel, client);
			break;
		case 401:
			errNoSuchNick(client, extra);
			break;
		case 403:
			errNoSuchChannel(client, extra);
			break;
		case 404:
			if (channel)
				errCannotSendToChan(*channel, client);
			break;
		case 431:
			errNoNickNameGiven(client);
			break;
		case 432:
			errErroneousNickName(client);
			break;
		case 433:
			errNickNameInUse(client);
			break;
		case 441:
			if (channel)
				errUserNotInChannel(client, *channel);
			break;
		case 442:
			if (channel)
				errNotOnChannel(client, *channel);
			break;
		case 443:
			if (channel)
				errUserOnChannel(client, *channel, *target);
			break;
		case 451:
			errNotRegistered(client, command);
			break;
		case 461:
			errNeedMoreParams(client, command);
			break;
		case 462:
			errAlreadyRegistered(client);
			break;
		case 464:
			errPasswordMismatch(client);
			break;
		case 471:
			if (channel)
				errChannelIsFull(*channel);
			break;
		case 473:
			if (channel)
				errInviteOnlyChan(*channel);
			break;
		case 475:
			if (channel)
				errBadChannelKey(*channel);
			break;
		case 482:
			if (channel)
				errChanOpPrivsNeeded(*channel, client);
			break;
		case 704:
			rplHelpStart(client, command);
			break;
        case 705:
			rplEndOfHelp(client, command);
			break;
		case 999:
			errUnknownCommand(client);
			break;
		default:
            client.sendBack("Unknown reply code: " + std::to_string(code));
	}
}


void Reply::rplWelcome(Client &client)
{
	client.sendBack("001 " + client.getNickname() + " :Welcome to the Internet Relay Network " + client.getNickname() + "!");
}

void Reply::rplIson(Client &client, Server &server)
{
	(void)client;
	(void)server;
}

void Reply::rplEndOfWho(Client &client)
{
	client.sendBack("315 " + client.getNickname() + " :End of /WHO list.");
}

void Reply::rplList(Client &client, Channel &channel)
{
	client.sendBack("322 * " + channel.getChannelName() + " " + std::to_string(channel.getMemberCount()) + " :" + channel.getTopic());
}

void Reply::rplListEnd(Client &client)
{
	client.sendBack("323 " + client.getNickname() + " :End of /LIST");
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

void Reply::rplInviting(Client &client, Channel &channel, Client &target, Server &server)
{
	target.sendMessage(":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getHostname() + " INVITE " + target.getNickname() + " :" + channel.getChannelName());
	client.sendBack("341 " + client.getNickname() + " " + target.getNickname() + " :" + channel.getChannelName(), "client");
}

void Reply::rplWhoReply(Channel &channel, Client &client)
{
	std::string userInfo = client.getFullIdentifier();
	client.sendBack("352 * " + channel.getChannelName() + " " + userInfo + " :H");
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

void Reply::errNoNickNameGiven(Client &client)
{
	client.sendBack("431 * :No nickname given");
}

void Reply::errErroneousNickName(Client &client)
{
	client.sendBack("432 " + client.getNickname() + " :Erroneous nickname");
}

void Reply::errNickNameInUse(Client &client)
{
	client.sendBack("433 " + client.getNickname() + " :Nickname is already in use");
}

void Reply::errUserNotInChannel(Client &client, Channel &channel)
{
	client.sendBack("441 " + client.getNickname() + " " + channel.getChannelName() + " :They aren't on that channel");
}

void Reply::errNotOnChannel(Client &client, Channel &channel)
{
	client.sendBack("442 " + client.getNickname() + " " + channel.getChannelName() + " :You're not on that channel");
}

void Reply::errUserOnChannel(Client &client, Channel &channel, Client &target)
{
	client.sendBack("443 " + client.getNickname() + " " + target.getNickname() + " " + channel.getChannelName() + " :is already on channel", "client");
}

void Reply::errNotRegistered(Client &client, std::string command)
{
	client.sendBack("451 " + client.getNickname() + " " + command + " :You have not registered", "client");
}

void Reply::errNeedMoreParams(Client &client, std::string command)
{
	client.sendBack("461 " + client.getNickname() + " " + command + " :Wrong number of parameters", "client");
}

void Reply::errAlreadyRegistered(Client &client)
{
	client.sendBack("462 " + client.getNickname() + " :You may not reregister");
}

void Reply::errPasswordMismatch(Client &client)
{
	client.sendBack("464 " + client.getNickname() + " :Password incorrect");
}

void Reply::errChannelIsFull(Channel &channel)
{
	channel.sendBack("471 * " + channel.getChannelName() + " :Cannot join channel (+l) - Channel is full");
}

void Reply::errInviteOnlyChan(Channel &channel)
{
	channel.sendBack("473 * " + channel.getChannelName() + " :Cannot join channel (+i)");
}

void Reply::errBadChannelKey(Channel &channel)
{
	channel.sendBack("475 * " + channel.getChannelName() + " :Cannot join channel (+k) - Wrong key");
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

void Reply::errUnknownCommand(Client &client)
{
	client.sendBack("999 " + client.getNickname() + " :Unknown command");
}

