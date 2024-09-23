#ifndef COMMANDS_NICK_HPP
#define COMMANDS_NICK_HPP

#include <vector>
#include <string>
#include "Command.hpp"

namespace Commands
{
	class Nick : public Command
	{
		public:

			Nick(std::vector<std::string> command_parts);

			virtual void 	execute(Client& client, Server& server);

		protected:

			std::string 	name;
			bool			error;
			std::string		errorMessage;
	};
}

#endif
