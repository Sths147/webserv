
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

		std::vector<char>	_buffer; // buffer when read and body if its was a POST
		bool				_header_saved;
		std::vector<char>	_header;

		// Request		_request;
		Response	_respons;

		Server		*_server;


		time_t		_time_to_reset; //sec
		Listen		_host_port;



		std::string	_response;

		void			find_server_from_map(std::vector<Server *> &vec_server);
	public:
		ClientFd( void );
		ClientFd(const Listen &listen);
		ClientFd &operator=( const ClientFd &other );
		void			print_vec(std::vector<char> &vec);
		bool			send_response( int client_fd );
		Listen			get_listen( void );
		void			refresh( void );
		bool			check_timeout( void );
		void			del_epoll_and_close( int epoll_fd, int client_fd );
		void			add_buffer( char *str, std::vector<Server *> &vec_server );





};

#endif
