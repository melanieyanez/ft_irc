#ifndef COMMANDS_MODE_HPP
#define COMMANDS_MODE_HPP

#include "Command.hpp"

#include <vector>
#include <string>
#include <map>

namespace Commands
{
	class Mode : public Command
	{
		public:

			Mode(const std::vector<std::string> &command_parts);

			virtual void 	execute(Client& client, Server& server);

		protected:
		
			std::string 						channelName;
			std::map<std::string, std::string> 	modeMap;
			std::string 						extraParam;

			void 								applyModes(Client &client, Server &server, Channel &channel);

	};
}

#endif
