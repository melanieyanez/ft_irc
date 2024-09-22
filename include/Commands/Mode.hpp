#ifndef COMMANDS_MODE_HPP
#define COMMANDS_MODE_HPP

#include <vector>
#include <string>
#include "Command.hpp"

namespace Commands
{
	class Mode : public Command
	{
		public:

			Mode(std::vector<std::string> command_parts);

			virtual void 	execute(Client& client, Server& server);

		protected:
		
			std::string 	channelName;
			std::string 	mode;
			std::string 	extraParam;
			bool 			error;
			std::string 	errorMessage;
	};
}

#endif
