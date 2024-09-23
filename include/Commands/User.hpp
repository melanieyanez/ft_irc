#ifndef COMMANDS_USER_HPP
#define COMMANDS_USER_HPP

#include "Command.hpp"

#include <vector>
#include <string>

namespace Commands
{
	class User : public Command
	{
		public:

			User(std::vector<std::string> command_parts);

			virtual void 	execute(Client& client, Server& server);

		protected:

			std::string 	name;
			std::string 	fullname;
	};
}

#endif
