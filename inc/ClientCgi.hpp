
#ifndef CLIENTCGI_HPP
#define CLIENTCGI_HPP


// #define TIMEOUT 5 //

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

		int					_fd_out;// pipe_out[0]
		std::string			_output_cgi;

		Response			*_response;
		int					_from_clientfd;



	public:

		ClientCgi(const int &in, const int &out, const int &client_fd);

		virtual ~ClientCgi( void );

		bool						read_cgi_output( void );
		bool						write_cgi_input( void );
		void						set_pid( pid_t &pid );
		void						add_body_request(const std::vector<char> & tmp);

		void						set_response(Response *res);
		void						construct_response( const int &epoll_fd, std::map<int, Client *> &fd_to_info );
		virtual void				del_epoll_and_close( int epoll_fd );
		bool						check_waitpid( pid_t &_pid );
		bool						check_timeout(const int &epoll_fd, std::map<int, Client *> &fd_to_info );
		int							get_fd();
		int							get_from_clientfd();


};

#endif
