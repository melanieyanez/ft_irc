#include "Channel.hpp"

#include <ctime>

Channel::Channel(std::string name, std::string key) : name(name), key(key), topic(""), memberLimit(0), invitationOnly(false), protection(false), topicRestricted(false), lastTopicSetter(""), lastTopicSetTime(std::time(0)){}

void Channel::sendMessage(std::string message, Client *sender) const
{
	for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		if (sender == NULL || *it != sender)  
			(*it)->sendMessage(message, "client");
	}
}

void Channel::sendBack(std::string reply) const
{
	for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
		(*it)->sendBack(reply);
}

std::string Channel::getChannelName() const
{
	return this->name;
}

bool Channel::isMember(Client &client) const
{
	for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		if ((*it)->getNickname() == client.getNickname())
			return true;
	}
	return false;
}

bool Channel::addMember(Client &client)
{
	// Si une limite de membres est définie et est atteinte, on refuse l'ajout
	if (memberLimit != 0 && members.size() >= memberLimit)
		return false;

	// Si le canal est vide, le premier membre devient opérateur
	if (members.empty())
		operators.push_back(&client);

	// Si le client n'est pas déjà membre, on l'ajoute
	if (!isMember(client))
		members.push_back(&client);

	return true;
}

std::vector<Client*> Channel::getMembers() const
{
	return members;
}

// Renvoie la liste des membres sous forme de chaîne, avec un "@" pour les opérateurs
std::string Channel::getMemberList() const
{
	std::string memberList;

	for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		if (isOperator(**it)) // Préfixe avec "@" si le membre est opérateur
			memberList += "@" + (*it)->getNickname() + " ";
		else
			memberList += (*it)->getNickname() + " ";
	}
	if (!memberList.empty())
		memberList.pop_back(); // Suppression de l'espace à la fin

	return memberList;
}

std::string Channel::getModes() const
{
	std::string modes = "+";

	if (this->isInvitationOnly()) modes += "i";
	if (this->isTopicRestricted()) modes += "t";
	if (this->getLimits() > 0) modes += "l";
	if (this->isProtected()) modes += "k";

	return modes;
}

void Channel::removeMember(Client &client)
{
	for (std::vector<Client*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		if ((*it)->getNickname() == client.getNickname())
		{
			members.erase(it);
			break;
		}
	}
	
	// Si le membre est également opérateur, on le supprime de la liste des opérateurs
	for (std::vector<Client*>::iterator it = operators.begin(); it != operators.end(); ++it)
	{
		if ((*it)->getNickname() == client.getNickname())
		{
			operators.erase(it);
			break;
		}
	}
}

void Channel::setTopic(const std::string &topic)
{
	this->topic = topic;
}

std::string Channel::getTopic() const
{
	return this->topic;
}

void Channel::invite(Client &client)
{
	invitedMembers.push_back(&client);
}

bool Channel::isInvited(const Client &client) const
{
	for (std::vector<Client*>::const_iterator it = invitedMembers.begin(); it != invitedMembers.end(); ++it)
	{
		if ((*it)->getNickname() == client.getNickname())
			return true;
	}
	return false;
}

bool Channel::isInvitationOnly() const
{
	return this->invitationOnly;
}

bool Channel::isProtected() const
{
	return this->protection;
}

bool Channel::isTopicRestricted() const
{
	return this->topicRestricted;
}

bool Channel::isOperator(const Client &client) const
{
	for (std::vector<Client*>::const_iterator it = operators.begin(); it != operators.end(); ++it)
	{
		if ((*it)->getNickname() == client.getNickname())
			return true;
	}
	return false;
}

void Channel::addOperator(Client &client)
{
	if (!isOperator(client))
		operators.push_back(&client);
}

void Channel::removeOperator(Client &client)
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

void Channel::setLimits(const unsigned int &limit)
{
	this->memberLimit = limit;
}

int Channel::getLimits() const
{
	return this->memberLimit;
}

unsigned int Channel::getMemberCount() const
{
	return this->members.size();
}

void Channel::setPassword(const std::string &key)
{
	this->key = key;
}

void Channel::setInvitationOnly(const bool &invitationOnly)
{
	this->invitationOnly = invitationOnly;
}

void Channel::setProtected(const bool &protection)
{
	this->protection = protection;
}

void Channel::setTopicRestricted(const bool &topicRestricted)
{
	this->topicRestricted = topicRestricted;
}


bool Channel::isCorrectKey(const std::string &key) const
{
	return this->key == key;
}

std::string Channel::getLastTopicSetter() const
{
	return this->lastTopicSetter;
}

void Channel::setLastTopicSetter(const std::string &nickName)
{
	this->lastTopicSetter = nickName;
}

std::string Channel::getLastTopicSetTime() const
{
	std::string timeStr = std::ctime(&this->lastTopicSetTime);
	
	if (!timeStr.empty() && timeStr.back() == '\n')
		timeStr.pop_back();
	return timeStr;
}

void Channel::setLastTopicSetTime()
{
	this->lastTopicSetTime = std::time(0);
}