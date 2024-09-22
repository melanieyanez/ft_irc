#ifndef COMMANDS_PRIVMSG_HPP
#define COMMANDS_PRIVMSG_HPP

#include <vector>
#include <string>
#include "Command.hpp"

namespace Commands
{
	class Privmsg : public Command
	{
	public:
		Privmsg(std::vector<std::string> command_parts);

		virtual void execute(Client& client, Server& server);

	protected:
		std::vector<std::string> 	recipients;
		std::string 				message;
		bool						error;
		std::string					errorMessage;
		size_t						MAX_MESSAGE_LENGTH;
	};
}

#endif
