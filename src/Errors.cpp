#include "Errors.hpp"

#include <string.h>

ReadError::ReadError(int error_code, Client& theClient) : std::runtime_error(strerror(error_code)), client(theClient)
{
}

Client& ReadError::getClient()
{
	return this->client;
}
