#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Channel.hpp"

#include <vector>
#include <string>

class Client;
class Channel;

class Server
{
	public:
		Server(std::string port, std::string password);
		~Server();

		void 						start();
		void 						stop();
		void 						handleCommand(std::string command, Client* creator);
		std::string 				getPassword();

		// rajouter pour join
		void 						addChannel(Channel* channel);
		Channel* 					getChannel(const std::string& channelName);

		std::vector<Client*> 		getClients();
		std::vector<Channel*> 		getChannels();

		void 						sendMessageToReceiver(std::string nickname, std::string message, Client& client);
		bool 						isNicknameConnected(std::string nickname);
		Client* 					getClientByNickname(std::string nickname);

		std::string 				getHostname();

	protected:
		std::vector<std::string> 	parseCommand(std::string command);
		void 						removeDisconnectedClient(struct pollfd fds[], int start_index, int clients_number);

	private:
		std::vector<Client*> 		clients;
		std::string 				port;
		std::string 				password;
		int 						fd;

		std::string 				channelName;
		std::vector<Channel*> 		channels;

		std::string 				hostname;
		bool 						stopRequested;

};

#endif
