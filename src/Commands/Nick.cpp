#include "Commands/Nick.hpp"

Commands::Nick::Nick(std::vector<std::string> command_parts)
{
	if (command_parts.size() != 2)
		throw ;

	this->name = command_parts[1];
}

void Commands::Nick::execute(Client& client, Server&)
{
	client.setNickname(name);
}
