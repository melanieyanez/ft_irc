#ifndef COMMANDS_INVITE_HPP
#define COMMANDS_INVITE_HPP

#include <vector>
#include <string>
#include "Command.hpp"

namespace Commands
{
	class Invite : public Command
	{
	public:
		Invite(std::vector<std::string> command_parts);

		virtual void execute(Client& client, Server& server);


	protected:
		std::string target;
		std::string channel;
		bool		error;
		std::string	errorMessage;
	};
}

#endif
