#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Channel.hpp"
#include "Server.hpp"
#include <vector>
#include <string>

class Server;
class Channel;

class Client
{
	public:

		Client(Server& server, int fd, const std::string &hostname);
		~Client();

		std::string getNickname() const;
		std::string getUsername() const;
		std::string getFullname() const;
		std::string getPassword() const;
		std::string getFullIdentifier() const;
		bool		getIsAuthenticated() const;

		bool		hasPass() const;
		bool		hasNick() const;

		void 		setNickname(const std::string &nickname);
		void 		setUsername(const std::string &username);
		void 		setFullname(const std::string &fullname);
		void 		setPassword(const std::string &password);
		void		authenticate();

		std::string readNextPacket();
		void 		sendBack(std::string reply, std::string target = "both") const;
		void 		sendMessage(std::string message, std::string target = "both") const;
		void 		closeConnection();

	private:

		std::string nickname;
		std::string username;
		std::string fullname;
		std::string password;
		int 		fd;

		bool		isAuthenticated;

		std::string line;
		Server		&server;
		std::string hostname;
};

#endif
