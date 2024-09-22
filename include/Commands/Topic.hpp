#ifndef COMMANDS_TOPIC_HPP
#define COMMANDS_TOPIC_HPP

#include <vector>
#include <string>
#include "Command.hpp"

namespace Commands
{
	class Topic : public Command
	{
		public:

			Topic(std::vector<std::string> command_parts);
			
			virtual void 	execute(Client& client, Server& server);

		protected:

			void 			setTopic(Client& client, Server& server, Channel& channel);
			void 			getCurrentTopic(Client& client, Server& server, Channel& channel);

			std::string 	channel;
			std::string 	topic;
			bool 			isSettingTopic;
			bool 			error;
			std::string 	errorMessage;
	};
}

#endif
