#ifndef COMMANDS_ISON_HPP
#define COMMANDS_ISON_HPP

#include "Command.hpp"

#include <vector>
#include <string>

namespace Commands
{
	class Ison : public Command
	{
		public:

			Ison(std::vector<std::string> command_parts);
			
			virtual void 				execute(Client& client, Server& server);

		protected:

			std::vector<std::string> 	users;
	};
}

#endif
