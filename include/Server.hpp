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
		Server(const std::string &port, const std::string &password);
		~Server();

		void 						start();
		void 						stop();
		void 						handleCommand(std::string command, Client* creator);
		std::string 				getPassword() const;

		void 						addChannel(Channel* channel);
		Channel* 					getChannel(const std::string& channelName) const;

		std::vector<Client*> 		getClients() const;
		std::vector<Channel*> 		getChannels() const;

		void 						sendMessageToReceiver(const std::string &nickname, const std::string &message, Client& client);
		bool 						isNicknameConnected(const std::string &nickname) const;
		Client* 					getClientByNickname(const std::string &nickname) const;

		std::string 				getHostname() const;

	protected:
		std::vector<std::string> 	parseCommand(std::string &command);
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
