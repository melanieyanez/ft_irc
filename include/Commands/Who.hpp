#ifndef COMMANDS_WHO_HPP
# define COMMANDS_WHO_HPP

#include "Command.hpp"

namespace Commands
{
	class Who : public Command
	{
		public:

			Who(const std::vector<std::string> &command_parts);

			virtual void 	execute(Client& Client, Server& Server);

		protected:

			std::string 	Target;
			void 			listConnectedUsers(Server &Server, Client &Client) const;
 			void 			listUsersInChannel(Server &Server, Client &Client, const std::string &channelName) const;
			void 			listSpecificUser(Server &Server, Client &Client, const std::string &nickName) const;
	};
};

#endif