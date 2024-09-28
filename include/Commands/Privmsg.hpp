#ifndef COMMANDS_PRIVMSG_HPP
#define COMMANDS_PRIVMSG_HPP

#include "Command.hpp"

#include <vector>
#include <string>

namespace Commands
{
	class Privmsg : public Command
	{
		public:

			Privmsg(const std::vector<std::string> &command_parts);
			
			virtual void 				execute(Client& client, Server& server);

		protected:

			std::vector<std::string> 	recipients;
			std::string 				message;
			size_t						MAX_MESSAGE_LENGTH;
	};
}

#endif
