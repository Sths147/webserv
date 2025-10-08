
#ifndef CLIENTFD_HPP
#define CLIENTFD_HPP


#define TIMEOUT 5 //

#include <ctime>
#include <iostream>

#include "Response.hpp"
#include "ConfigUtils.hpp"
#include "Client.hpp"


class ClientFd : public Client
{
	private:

		Listen				_host_port;

		bool				_body_saved;
		std::vector<char>	_buffer; // buffer when read and body if its was a POST
		bool				_header_saved;
		std::vector<char>	_header;

		Request				*_request;
		Server				*_server;
		bool				_alive;
		std::string			_response;



		void			_abstrait(void);
		void			find_server_from_map(std::vector<Server *> &vec_server);

	public:
		ClientFd( void );
		ClientFd(const Listen &listen);
		ClientFd &operator=( const ClientFd &other );
		~ClientFd( void );

		Listen				get_listen( void );



		const std::string	get_type() const;
		bool				get_body_check( void );
		bool				get_header_saved( void );
		bool				check_alive( void );

		void				print_vec(std::vector<char> &vec);
		void				add_buffer( char *str, std::vector<Server *> &vec_server );

		void				creat_response( void );
		bool				send_response( int client_fd );


		void				del_epoll_and_close( int epoll_fd, int client_fd );

};

#endif
