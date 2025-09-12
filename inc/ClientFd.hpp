
#ifndef CLIENTFD_HPP
#define CLIENTFD_HPP


#define TIMEOUT 30 //

#include <ctime>
#include <iostream>

#include "ConfigUtils.hpp"

class ClientFd
{
	private:
		int			_fd;
		time_t		_time_to_reset; //sec
		Listen		_host_port;

	public:
		ClientFd( void );
		ClientFd(int fd, Listen &listen);
		int		get_fd( void );
		Listen	get_listen( void );
		void	refresh( void );
		bool	check_timeout( void );
		void	del_epoll_and_close( int epoll_fd );


};

#endif
