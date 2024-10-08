#ifndef COMMANDS_LIST_HPP
# define COMMANDS_LIST_HPP

#include "Command.hpp"

namespace Commands
{
	class List : public Command
	{
		public:

			List(const std::vector<std::string> &command_parts);
			
			virtual void 	execute(Client& Client, Server& Server);
	};
};

#endif