#include "Commands/Pass.hpp"

#include <iostream>

Commands::Pass::Pass(std::vector<std::string> command_parts)
{
	this->password = command_parts[1];
}


void Commands::Pass::execute(Client& client, Server&)
{
	client.setPassword(password);
}
