#ifndef COMMANDS_JOIN_HPP
#define COMMANDS_JOIN_HPP

#include "Command.hpp"

#include <vector>
#include <string>

namespace Commands
{
	class Join : public Command
	{
		public:

			Join(const std::vector<std::string> &command_parts);
			
			virtual void 				execute(Client& client, Server& server);

		protected:

			std::string 				channelName;
			std::vector<std::string> 	channels;
			std::vector<std::string> 	keys;
	};

}
#endif
