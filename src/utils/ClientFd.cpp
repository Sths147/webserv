
#include "ClientFd.hpp"

#define RESET "\033[0m"
#define RED "\033[31m"

ClientFd::ClientFd( void ):
_fd(0){}

ClientFd::ClientFd( int fd , Listen &listen ) :
_fd(fd),
_time_to_reset(std::time(NULL) + TIMEOUT),
_host_port(listen.ip, listen.port) {}


int		ClientFd::get_fd( void ) { return (this->_fd); }
Listen	ClientFd::get_listen( void ) { return (this->_host_port); }


void	ClientFd::refresh( void ) { this->_time_to_reset = std::time(NULL) + TIMEOUT; }


bool	ClientFd::check_timeout( void ) {
	if (this->_time_to_reset < std::time(NULL)){
		std::cout << RED << " timeout" << RESET;
		return (false);
	}
	return (true);
}
void	ClientFd::set_response(Response res) {
	this->_res = res;
}

void	ClientFd::send(int fd ){
	std:: cout << "BODY|" << this->_res.get_body() << "|" << std::endl;
	std::cout << "--------NOW DISPLAYING HEADERS---------" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = this->_res.get_headers().begin(); it != this->_res.get_headers().end(); it++)
		std::cout << it->first << "--" << it->second << std::endl;
	std::cout << "--------END DISPLAYING HEADERS---------" << std::endl;
	this->_res.write_response(fd);
}

#include <unistd.h>
#include <sys/epoll.h>
void	ClientFd::del_epoll_and_close( int epoll_fd ) {
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, this->_fd, NULL);
	close(this->_fd);
}
