#include "Client.hpp"


Client::~Client( void ) {}
Client::Client( void ) : _time_to_reset(std::time(NULL) + TIMEOUT) {}

Client::Client( const Client &other) : _time_to_reset(std::time(NULL) + TIMEOUT) {
	this->_time_to_reset = other._time_to_reset;
}

Client &Client::operator=( const Client &other ) {
	if (this != &other)	{
		this->_time_to_reset = other._time_to_reset;
	}
	return (*this);
}

/*----timeout----*/

void	Client::refresh( void ) { this->_time_to_reset = std::time(NULL) + TIMEOUT; }

bool	Client::check_timeout( void ) {
	if (this->_time_to_reset < std::time(NULL)) {
		std::cout << RED << "Timeout" << RESET<<std::endl;
		return (false);
	}
	return (true);
}

/*----timeout----*/
