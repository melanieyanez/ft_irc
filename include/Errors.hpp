#ifndef ERRORS_H
#define ERRORS_H

#include "Client.hpp"

#include <stdexcept>

class ReadError : public std::runtime_error
{
public:
	ReadError(int error_code, Client& client);
	Client& getClient();

private:
	Client& client;

};

#endif
