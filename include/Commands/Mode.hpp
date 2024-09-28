#ifndef COMMANDS_MODE_HPP
#define COMMANDS_MODE_HPP

#include "Command.hpp"

#include <vector>
#include <string>

namespace Commands
{
	class Mode : public Command
	{
		public:

			Mode(const std::vector<std::string> &command_parts);

			virtual void 	execute(Client& client, Server& server);

		protected:
		
			std::string 	channelName;
			std::string 	mode;
			std::string 	extraParam;
	};
}

#endif
