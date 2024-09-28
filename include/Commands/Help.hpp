#ifndef COMMANDS_HELP_HPP
# define COMMANDS_HELP_HPP

#include "Command.hpp"

namespace Commands
{
	class Help : public Command
	{
		public:

			Help(const std::vector<std::string> &command_parts);
			
			virtual void 	execute(Client& client, Server& server);

		protected:

			std::string 	command;
			std::string		generateLoginHelp() const;
			std::string 	generateGeneralHelp() const;
			std::string 	generateCommandHelp(const std::string &command) const;
	};
};

#endif