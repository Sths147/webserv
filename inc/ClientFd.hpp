
#ifndef CLIENTFD_HPP
#define CLIENTFD_HPP


#define TIMEOUT 5 //

#include <ctime>
#include <iostream>

#include "Response.hpp"
#include "ConfigUtils.hpp"

class ClientFd
{
	private:

		std::vector<char>	_buffer; // buffer when read

		time_t		_time_to_reset; //sec
		Listen		_host_port;

		std::vector<char>	_header;
		Response	_res;


		std::string	_response;

	public:
		ClientFd( void );
		ClientFd(const Listen &listen);
		ClientFd &operator=( const ClientFd &other );
		bool			send_response( int client_fd );
		Listen			get_listen( void );
		void			refresh( void );
		bool			check_timeout( void );
		void			del_epoll_and_close( int epoll_fd, int client_fd );
		void			add_buffer( std::string str);
		// ClientFd &operator+=( std::string str);


};

#endif
