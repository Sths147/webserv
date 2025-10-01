
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

		time_t				_time_to_reset; //sec
		Listen				_host_port;

		bool				_body_check;
		std::vector<char>	_buffer; // buffer when read and body if its was a POST
		bool				_header_saved;
		std::vector<char>	_header;

		// Request				_request;
		Response			_respons;
		Server				*_server;


		std::string	_response; // to delete

		void			find_server_from_map(std::vector<Server *> &vec_server);

		public:
		ClientFd( void );
		ClientFd(const Listen &listen);
		ClientFd &operator=( const ClientFd &other );

		Listen			get_listen( void );

		/*----timeout----*/
		void			refresh( void );
		bool			check_timeout( void );
		/*----timeout----*/

		bool			get_body_check( void );
		bool			get_header_saved( void );

		void			print_vec(std::vector<char> &vec);
		void			add_buffer( char *str, std::vector<Server *> &vec_server );
		bool			send_response( int client_fd );


		void			del_epoll_and_close( int epoll_fd, int client_fd );

};

#endif
