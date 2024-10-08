#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Client.hpp"

class Command
{
	public:
		Command() : error(false), errorCode(0){}
		virtual void 	execute(Client& client, Server& server) = 0;

	protected:
		bool 			error;
		int				errorCode;

};

#endif
