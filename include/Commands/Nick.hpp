#ifndef COMMANDS_NICK_HPP
#define COMMANDS_NICK_HPP

#include "Command.hpp"

#include <vector>
#include <string>

namespace Commands
{
	class Nick : public Command
	{
		public:

			Nick(std::vector<std::string> command_parts);

			virtual void 	execute(Client& client, Server& server);

		protected:

			std::string 	name;
	};
}

#endif
