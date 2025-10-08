#include "Client.hpp"




Client::Client( void ) : _time_to_reset(std::time(NULL) + TIMEOUT) {

}
Client::Client( const Client &other) : _time_to_reset(std::time(NULL) + TIMEOUT) {
	(void)other;
}

Client &Client::operator=( const Client &other ){
	(void)other;
	return (*this);
}
Client::~Client( void ) {

}

/*----timeout----*/

void	Client::refresh( void ) { this->_time_to_reset = std::time(NULL) + TIMEOUT; }

bool	Client::check_timeout( void ) {
	if (this->_time_to_reset < std::time(NULL)){
		std::cout << RED << "Timeout" << RESET<<std::endl;
		return (false);
	}
	return (true);
}

/*----timeout----*/
