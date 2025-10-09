
#ifndef CLIENTCGI_HPP
#define CLIENTCGI_HPP


#define TIMEOUT 5 //

#include <ctime>
#include <iostream>
#include <vector>
#include "Client.hpp"
#include "Response.hpp"

class Response;

class ClientCgi : public Client
{
	private:

		pid_t				_pid;

		int					_fd_in;// pipe_in[1]
		std::string			_body_request;
		bool				_write_finish;

		int					_fd_out;// pipe_out[0]
		std::string			_output_cgi;
		bool				_read_finish;

		Response			*_response;
		int					_from_clientfd;



	public:
		ClientCgi( void );

		ClientCgi(const int &in, const int &out, const int &client_fd);


		ClientCgi &operator=( const ClientCgi &other );
		virtual ~ClientCgi( void );

		int							read_cgi_output( void );
		int							write_cgi_input( void );
		void						set_pid( pid_t &pid );
		void						add_body_request(const std::vector<char> & tmp);

		void						set_response(Response *res);
		void						construct_response( const int &epoll_fd, std::map<int, Client *> &fd_to_info );
		virtual void				del_epoll_and_close( int epoll_fd );

};

#endif
