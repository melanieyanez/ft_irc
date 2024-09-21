#ifndef COMMANDS_HELP_HPP
# define COMMANDS_HELP_HPP

#include "Command.hpp"

namespace Commands
{
	class Help : public Command
	{
		public:
			Help(std::vector<std::string> command_parts);
			virtual void execute(Client& client, Server& server);

		protected:
			std::string command;
			bool error;
			std::string errorMessage;
			std::string generateGeneralHelp();
			std::string generateCommandHelp(const std::string &command);
	};
};

#endif