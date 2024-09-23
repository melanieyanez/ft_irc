#ifndef COMMANDS_INVITE_HPP
#define COMMANDS_INVITE_HPP

#include "Command.hpp"

#include <vector>
#include <string>

namespace Commands
{
	class Invite : public Command
	{
		public:

			Invite(std::vector<std::string> command_parts);
			
			virtual void 	execute(Client& client, Server& server);

		protected:

			std::string 	target;
			std::string 	channel;
	};
}

#endif
