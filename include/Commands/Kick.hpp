#ifndef COMMANDS_KICK_HPP
#define COMMANDS_KICK_HPP

#include "Command.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"

#include <vector>
#include <string>

namespace Commands
{
	class Kick : public Command
	{
		public:

			Kick(std::vector<std::string> command_parts);
			
			virtual void 	execute(Client& client, Server& server);

		protected:

			std::string 	channelName;
			std::string 	nickname;
			std::string 	reason;
	};
}

#endif
