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
	Client(Server& server, int fd, std::string hostname);
	~Client();

	std::string getNickname();
	std::string getUsername();
	std::string getFullname();
	std::string getPassword();
	std::string getFullIdentifier();

	void setNickname(std::string nickname);
	void setUsername(std::string username);
	void setFullname(std::string fullname);
	void setPassword(std::string password);

	std::string readNextPacket();
	void sendBack(std::string reply, std::string target = "both");
	void sendMessage(std::string message);

	void closeConnection();

private:
	std::string nickname;
	std::string username;
	std::string fullname;
	std::string password;
	int fd;

	std::string line;
	Server& server;
	std::string hostname;

};

#endif
