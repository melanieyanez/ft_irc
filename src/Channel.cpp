#include "Channel.hpp"
#include <ctime>

// pas fini =====================
Channel::Channel(std::string name, std::string key) : name(name), key(key)
{
	this->invitationOnly = false;
	this->topicRestricted = false;
}

void Channel::sendBack(std::string reply)
{
	for (std::vector<Client*>::iterator it = members.begin(); it != members.end(); ++it)
		(*it)->sendBack(reply);
}

std::string Channel::getChannelName()
{
	return this->name;
}

bool Channel::isMember(Client& client)
{
	for (std::vector<Client*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		if ((*it)->getNickname() == client.getNickname())
			return true;
	}
	return false;
}

// pas fini =====================
bool Channel::addMember(Client& client)
{
	// check si les membres sont limite == a controler, pas sur que ca soit la bonne place
	if (memberLimit != 0 && members.size() >= memberLimit)
		return false;

	if (members.empty())
		operators.push_back(&client);

	if (!isMember(client))
		members.push_back(&client);

	return true;
}

// pas fini =====================
std::vector<Client*> Channel::getMembers()
{
	return members;
}

std::string Channel::getMemberList()
{
	std::string memberList;

	for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		if (isOperator(**it))
			memberList += "@" + (*it)->getNickname() + " ";
		else
			memberList += (*it)->getNickname() + " ";
	}
	if (!memberList.empty())
		memberList.pop_back();

	return memberList;
}

void Channel::removeMember(Client& client)
{
	for (std::vector<Client*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		if ((*it)->getNickname() == client.getNickname())
		{
			members.erase(it);
			break;
		}
	}
	for (std::vector<Client*>::iterator it = operators.begin(); it != operators.end(); ++it)
	{
		if ((*it)->getNickname() == client.getNickname())
		{
			operators.erase(it);
			break;
		}
	}
}

void Channel::setTopic(std::string topic)
{
	this->topic = topic;
}

std::string Channel::getTopic()
{
	return this->topic;
}

void Channel::invite(Client& client)
{
	invitedMembers.push_back(&client);
}

bool Channel::isInvited(Client& client)
{
	for (std::vector<Client*>::iterator it = invitedMembers.begin(); it != invitedMembers.end(); ++it)
	{
		if ((*it)->getNickname() == client.getNickname())
			return true;
	}
	return false;
}

bool Channel::isInvitationOnly()
{
	return this->invitationOnly;
}

bool Channel::isProtected()
{
	return this->protection;
}

bool Channel::isTopicRestricted()
{
	return this->topicRestricted;
}

bool Channel::isOperator(Client& client)
{
	for (std::vector<Client*>::iterator it = operators.begin(); it != operators.end(); ++it)
	{
		if ((*it)->getNickname() == client.getNickname())
			return true;
	}
	return false;
}

void Channel::addOperator(Client& client)
{
	if (!isOperator(client))
		operators.push_back(&client);
}

void Channel::removeOperator(Client& client)
{
	for (std::vector<Client*>::iterator it = operators.begin(); it != operators.end(); ++it)
	{
		if ((*it)->getNickname() == client.getNickname())
		{
			operators.erase(it);
			break;
		}
	}
}

void Channel::setLimits(unsigned int limit)
{
	this->memberLimit = limit;
}

unsigned int Channel::getLimits()
{
	return this->memberLimit;
}

unsigned int Channel::getMemberCount() const
{
	return this->members.size();
}

void Channel::setPassword(std::string key)
{
	this->key = key;
}

void Channel::setInvitationOnly(bool invitationOnly)
{
	this->invitationOnly = invitationOnly;
}

void Channel::setProtected(bool protection)
{
	this->protection = protection;
}

void Channel::setTopicRestricted(bool topicRestricted)
{
	this->topicRestricted = topicRestricted;
}


bool Channel::isCorrectKey(std::string key)
{
	return this->key == key;
}

std::string Channel::getLastTopicSetter()
{
	return this->lastTopicSetter;
}

void Channel::setLastTopicSetter(std::string nickName)
{
	this->lastTopicSetter = nickName;
}

std::string Channel::getLastTopicSetTime()
{
	return std::string(std::ctime(&this->lastTopicSetTime));
}

void Channel::setLastTopicSetTime()
{
	this->lastTopicSetTime = std::time(0);
}