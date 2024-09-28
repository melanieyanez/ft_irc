#ifndef REPLY_HPP
#define REPLY_HPP

#include <string>

#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"

class Reply
{
	public:

		void sendReply(const int &code, Client &client, Client *target, Channel *channel = NULL, const std::string &command = "", const std::string &extra = "")  const;

	private:

		void rplWelcome(const Client &client) const;
		void rplIson(const Client &client, const std::string &extra) const;
		void rplEndOfWho(const Client &client) const;
		void rplList(const Client &client, const Channel &channel) const;
		void rplListEnd(const Client &client) const;
		void rplNoTopic(const Client &client, const Channel &channel) const;
		void rplTopic(const Client &client, const Channel &channel) const;
		void rplTopicWhoTime(const Channel &channel, const Client &client) const;
		void rplInviting(const Client &client, const Channel &channel, const Client &target) const;
		void rplWhoReply(const Channel &channel, const Client &client, const Client &target, const std::string &command) const;
		void rplNamReply(const Client &client, const Channel &channel) const;
		void rplEndOfNames(const Client &client, const Channel &channel) const;
		void rplHelpStart(const Client &client, const std::string &command) const;
		void rplEndOfHelp(const Client &client, const std::string &command) const;

		void errNoSuchNick(const Client &client, const std::string &extra, const std::string &command) const;
		void errNoSuchChannel(const Client &client, const std::string &command, const std::string &extra) const;
		void errCannotSendToChan(const Channel &channel, const Client &client, const std::string &command) const;
		void errNoNickNameGiven(const Client &client, const std::string &command) const;
		void errErroneousNickName(const Client &client, const std::string &command) const;
		void errNickNameInUse(const Client &client, const std::string &command) const;
		void errUserNotInChannel(const Client &client, const Channel &channel, const Client &target, const std::string &command) const;
		void errNotOnChannel(const Client &client, const Channel &channel, const std::string &command) const;
		void errUserOnChannel(const Client &client, const Channel &channel, const std::string &extra, const std::string &command) const;
		void errNotRegistered(const Client &client, const std::string &command) const;
		void errNeedMoreParams(const Client &client, const std::string &command) const;
		void errAlreadyRegistered(const Client &client, const std::string &command) const;
		void errPasswordMismatch(const Client &client, const std::string &command) const;
		void errChannelKeyAlreadySet(const Client &client, const Channel &channel) const;
		void errChannelIsFull(const Client &client, const Channel &channel, const std::string &command) const;
		void errInvalidMode(const Client &client, const Channel &channel, const std::string &command) const;
		void errInviteOnlyChan(const Client &client, const Channel &channel, const std::string &command) const;
		void errBadChannelKey(const Client &client, const Channel &channel, const std::string &command) const;
		void errBadChannelMask(const Client &client, const std::string &channel) const;
		void errChanOpPrivsNeeded(const Channel &channel, const Client &client, const std::string &command) const;
		void errUnknownCommand(const Client &client, const std::string &command) const;
};

#endif
