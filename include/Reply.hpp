#ifndef REPLY_HPP
#define REPLY_HPP

#include <string>

#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"

class Reply
{
	public:

		void sendReply(int code, Client &client, Client *target, Channel *channel = NULL, Server *server = NULL, const std::string command = "", const std::string extra = "");

	private:

		void rplWelcome(Client &client);
		void rplIson(Client &client, Server &server);
		void rplEndOfWho(Client &client);
		void rplList(Client &client, Channel &channel);
		void rplListEnd(Client &client);
		void rplNoTopic(Client &client, Channel &channel);
		void rplTopic(Client &client, Channel &channel);
		void rplTopicWhoTime(Channel &channel, Client &client);
		void rplInviting(Client &client, Channel &channel, Client &target, Server &server);
		void rplWhoReply(Channel &channel, Client &client);
		void rplNameReply(Channel &channel);
		void rplEndOfNames(Channel &channel);
		void errNoSuchNick(Client &client, std::string extra);
		void errNoSuchChannel(Client &client, std::string extra);
		void errCannotSendToChan(Channel &channel, Client &client);
		void errNoNickNameGiven(Client &client);
		void errErroneousNickName(Client &client);
		void errNickNameInUse(Client &client);
		void errUserNotInChannel(Client &client, Channel &channel);
		void errNotOnChannel(Client &client, Channel &channel);
		void errUserOnChannel(Client &client, Channel &channel, Client &target);
		void errNotRegistered(Client &client, std::string command);
		void errNeedMoreParams(Client &client, std::string command);
		void errAlreadyRegistered(Client &client);
		void errPasswordMismatch(Client &client);
		void errChannelIsFull(Channel &channel);
		void errInviteOnlyChan(Channel &channel);
		void errBadChannelKey(Channel &channel);
		void errChanOpPrivsNeeded(Channel &channel, Client &client);
		void rplHelpStart(Client &client, const std::string command);
		void rplEndOfHelp(Client &client, const std::string command);
		void errUnknownCommand(Client &client);
};

#endif
