#ifndef COMMANDS_PART_HPP
# define COMMANDS_PART_HPP

#include "Command.hpp"

namespace Commands
{
	class Part : public Command
	{
		public:

			Part(const std::vector<std::string> &command_parts);
			
			virtual void 				execute(Client& client, Server& server);

		protected:

			std::vector<std::string> 	channels;
			std::string					partMessage;
	};
};

#endif