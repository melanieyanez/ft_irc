#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"

#include <string>
#include <vector>

class Client;

class Channel
{
	public:

		Channel(std::string name, std::string password);

		bool 					isMember(Client &client) const;
		bool 					addMember(Client &client);
		void 					removeMember(Client &client);

		void 					sendMessage(std::string message) const;
		void 					sendBack(std::string reply) const;

		void 					setTopic(const std::string &topic);
		void 					setLimits(const unsigned int &limit);
		void 					setPassword(const std::string &password);
		void 					setInvitationOnly(const bool &invitationOnly);
		void					setProtected(const bool &protection);
		void 					setTopicRestricted(const bool &topicRestricted);
		void 					setLastTopicSetter(const std::string &nickName);
		void 					setLastTopicSetTime();

		std::string 			getChannelName() const;
		std::vector<Client*> 	getMembers() const;
		std::string 			getMemberList() const;
		std::string 			getTopic() const;
		int 					getLimits() const;
		unsigned int 			getMemberCount() const;
		std::string 			getLastTopicSetter() const;
		std::string 			getLastTopicSetTime() const;



		void 					invite(Client &client);
		bool 					isInvited(const Client &client) const;
		bool 					isInvitationOnly() const;
		bool					isProtected() const;
		bool 					isTopicRestricted() const;
		bool 					isCorrectKey(const std::string &key) const;

		bool 					isOperator(const Client &client) const;
		void 					addOperator(Client &client);
		void 					removeOperator(Client &client);


	private:
	
		std::string 			name;
		std::string 			key;
		std::vector<Client*> 	operators;
		std::vector<Client*> 	members;
		std::vector<Client*> 	invitedMembers;
		std::string 			topic;
		unsigned int 			memberLimit;
		bool 					invitationOnly;
		bool					protection;
		bool 					topicRestricted;
		std::string 			lastTopicSetter;
		std::time_t 			lastTopicSetTime;
};

#endif