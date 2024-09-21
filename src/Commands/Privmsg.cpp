#include "Commands/Privmsg.hpp"
#include "Server.hpp"
#include <sstream>

Commands::Privmsg::Privmsg(std::vector<std::string> command_parts)
{
	if (command_parts.size() != 3)
		throw ;

	// split for multiple targets
	std::stringstream stream(command_parts[1]);
	std::string segment;
	while(std::getline(stream, segment, ','))
		names.push_back(segment);

	this->message = command_parts[2];
}

void Commands::Privmsg::execute(Client& client, Server& server)
{	
	for (std::vector<std::string>::iterator it = names.begin(); it != names.end(); ++it)
		server.sendMessageToReceiver(*it, ":" + message, client);
}
