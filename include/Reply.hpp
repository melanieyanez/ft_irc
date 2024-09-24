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
		void rplIson(Client &client, std::string extra);
		void rplEndOfWho(Client &client);
		void rplList(Client &client, Channel &channel);
		void rplListEnd(Client &client);
		void rplNoTopic(Client &client, Channel &channel);
		void rplTopic(Client &client, Channel &channel);
		void rplTopicWhoTime(Channel &channel, Client &client);
		void rplInviting(Client &client, Channel &channel, Client &target);
		void rplWhoReply(Channel &channel, Client &client);
		void rplNamReply(Client &client, Channel &channel);
		void rplEndOfNames(Client &client, Channel &channel);
		void rplHelpStart(Client &client, const std::string command);
		void rplEndOfHelp(Client &client, const std::string command);

		void errNoSuchNick(Client &client, std::string extra, std::string command);
		void errNoSuchChannel(Client &client, std::string command, std::string extra);
		void errCannotSendToChan(Channel &channel, Client &client, std::string command);
		void errNoNickNameGiven(Client &client, std::string command);
		void errErroneousNickName(Client &client, std::string command);
		void errNickNameInUse(Client &client, std::string command);
		void errUserNotInChannel(Client &client, Channel &channel, Client &target, std::string command);
		void errNotOnChannel(Client &client, Channel &channel, std::string command);
		void errUserOnChannel(Client &client, Channel &channel, std::string extra, std::string command);
		void errNotRegistered(Client &client, std::string command);
		void errNeedMoreParams(Client &client, std::string command);
		void errAlreadyRegistered(Client &client, std::string command);
		void errPasswordMismatch(Client &client, std::string command);
		void errChannelIsFull(Client &client, Channel &channel, std::string command);
		void errInviteOnlyChan(Client &client, Channel &channel, std::string command);
		void errBadChannelKey(Client &client, Channel &channel, std::string command);
		void errChanOpPrivsNeeded(Channel &channel, Client &client, std::string command);
		void errUnknownCommand(Client &client, const std::string command);
};

#endif
