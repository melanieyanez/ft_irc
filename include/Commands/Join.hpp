#ifndef COMMANDS_JOIN_HPP
#define COMMANDS_JOIN_HPP

#include <vector>
#include <string>
#include "Command.hpp"

namespace Commands
{
	class Join : public Command
	{
		public:

			Join(std::vector<std::string> command_parts);
			
			virtual void 				execute(Client& client, Server& server);

		protected:

			std::string 				channelName;
			std::vector<std::string> 	channels;
			std::vector<std::string> 	keys;
			bool						error;
			std::string					errorMessage;
	};

}
#endif
