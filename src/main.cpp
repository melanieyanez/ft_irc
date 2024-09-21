#include "Server.hpp"
#include <iostream>
#include <csignal>

Server* g_server = NULL;

void signal_handler(int)
{
	g_server->stop();
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Bad arguments : " << argv[0] << " <port> <password>" << std::endl;
		return 1;
	}

	try
	{
		std::signal(SIGINT, signal_handler);
		std::signal(SIGTERM, signal_handler);
		g_server = new Server(argv[1], argv[2]);
		g_server->start();
		delete g_server;
	}
	catch(const std::exception& e)
	{
		std::cerr << "main(): caught: " << e.what() << '\n';
		delete g_server;
		return 1;
	}

	return 0;
}
