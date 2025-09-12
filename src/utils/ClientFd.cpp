
#include "ClientFd.hpp"


ClientFd::ClientFd( void ){}

ClientFd::ClientFd( int fd , Listen &listen ) :
_fd(fd),
_time_to_reset(std::time(NULL) + TIMEOUT),
_host_port(listen.ip, listen.port) {}


int		ClientFd::get_fd( void ) { return (this->_fd); }
Listen	ClientFd::get_listen( void ) { return (this->_host_port); }


void	ClientFd::refresh( void ) { this->_time_to_reset = std::time(NULL); }


bool	ClientFd::check_timeout( void ) {
	if (this->_time_to_reset < std::time(NULL))
		return (false);
	return (true);
}


#include <unistd.h>
#include <sys/epoll.h>
void	ClientFd::del_epoll_and_close( int epoll_fd ) {
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, this->_fd, NULL);
	close(this->_fd);
}
