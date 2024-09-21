#ifndef COMMANDS_PASS_HPP
#define COMMANDS_PASS_HPP

#include <vector>
#include <string>
#include "Command.hpp"

namespace Commands
{
	class Pass : public Command
	{
	public:
		Pass(std::vector<std::string> command_parts);

		virtual void execute(Client& client, Server& server);

	protected:
		std::string password;


	};
}

#endif
