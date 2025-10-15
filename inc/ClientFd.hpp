
#ifndef CLIENTFD_HPP
#define CLIENTFD_HPP


// #define TIMEOUT 5 //

#include <ctime>
#include <iostream>

#include "Response.hpp"
#include "ConfigUtils.hpp"
#include "Client.hpp"


class ClientFd : public Client
{
	private:

		int					_fd;
		Listen				_host_port;

		bool				_body_saved;
		std::vector<char>	_buffer; // buffer when read and body if its was a POST
		bool				_header_saved;
		std::vector<char>	_header;

		Request				*_request;
		Response			*_res;
		Server				*_server;
		bool				_alive;

		std::string			_response;
		int					_epoll_fd;

		// ClientCgi			*_cgi_fd1;
		// ClientCgi			*_cgi_fd2;


		void			find_server_from_map(std::vector<Server *> &vec_server);

	public:
		ClientFd( void );
		ClientFd(const Listen &listen, int fd, int epoll_fd);
		ClientFd &operator=( const ClientFd &other );
		~ClientFd( void );
		void				clean_new_request( void );
		// void				clean_cgi( const int &epoll_fd, std::map<int, Client *> &fd_to_info );
		void				del_epoll_and_close( int epoll_fd );

		Listen				get_listen( void );



		const std::string	get_type() const;
		bool				get_body_check( void );
		bool				get_header_saved( void );
		bool				check_alive( void );

		void				print_vec(std::vector<char> &vec);
		void				add_buffer( char *str, std::vector<Server *> &vec_server, size_t bytes_read );

		int					creat_response( std::map<int, Client *> &fd_to_info, std::vector<Server *> &vec_server);
		void				set_response_str( const std::string &str);
		bool				send_response( int client_fd );


};

#endif
