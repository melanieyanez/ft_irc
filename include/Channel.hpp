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

	bool 					isMember(Client& client);
	bool 					addMember(Client& client);
	void 					removeMember(Client& client);

	void 					sendBack(std::string reply);

	void 					setTopic(std::string topic);
	void 					setLimits(unsigned int limit);
	void 					setPassword(std::string password);
	void 					setInvitationOnly(bool invitationOnly);
	void 					setTopicRestricted(bool topicRestricted);
	void 					setLastTopicSetter(std::string nickName);
	void 					setLastTopicSetTime();

	std::string 			getChannelName();
	std::vector<Client*> 	getMembers();
	std::string 			getTopic();
	unsigned int 			getLimits();
	unsigned int 			getMemberCount() const;
	std::string 			getLastTopicSetter();
	std::string 			getLastTopicSetTime();



	void 					invite(Client& client);
	bool 					isInvited(Client& client);
	bool 					isInvitationOnly();
	bool 					isTopicRestricted();
	bool 					isCorrectKey(std::string key);

	bool 					isOperator(Client& client);
	void 					addOperator(Client& client);
	void 					removeOperator(Client& client);


private:
	std::string 			name;
	std::string 			key;
	std::vector<Client*> 	operators;
	std::vector<Client*> 	members;
	std::vector<Client*> 	invitedMembers;
	std::string 			topic;
	unsigned int 			memberLimit;
	bool 					invitationOnly;
	bool 					topicRestricted;
	std::string 			lastTopicSetter;
	std::time_t 			lastTopicSetTime;
};

#endif