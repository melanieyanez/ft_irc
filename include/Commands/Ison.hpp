#ifndef COMMANDS_ISON_HPP
#define COMMANDS_ISON_HPP

#include <vector>
#include <string>
#include "Command.hpp"

namespace Commands
{
	class Ison : public Command
	{
		public:

			Ison(std::vector<std::string> command_parts);
			
			virtual void 				execute(Client& client, Server& server);

		protected:

			std::vector<std::string> 	users;
			bool 						error;
			std::string 				errorMessage;
	};
}

#endif
