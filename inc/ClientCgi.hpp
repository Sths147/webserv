
#ifndef CLIENTCGI_HPP
#define CLIENTCGI_HPP


#define TIMEOUT 5 //

#include <ctime>
#include <iostream>
#include "Client.hpp"


class ClientCgi : protected Client
{
	private:

		int					_fd_in; // pipe_in[1]
		std::string			_body_request;
		int					_fd_out; // pipe_out[0]
		std::string			_output_cgi;



	public:
		ClientCgi( void );
		ClientCgi(const int in, const int out, std::string _body_request);
		ClientCgi &operator=( const ClientCgi &other );
		virtual ~ClientCgi( void );
		virtual void				del_epoll_and_close( int epoll_fd );

};

#endif
