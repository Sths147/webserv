#include "Server.hpp"
#include "MyException.hpp"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

Server::Server( void )
{

}

static void set_nonblocking(int socket_fd) {
	int flags = fcntl(socket_fd, F_GETFL, 0);
	fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
}

static void set_address(struct sockaddr_in	&address, Listen &listen) {
	// std::cout << "listen ip : " << listen.ip << std::endl;
	// std::cout << "listen port : " << listen.port << std::endl;
	address.sin_family = AF_INET;
	if (listen.ip == 0){
		address.sin_addr.s_addr =  INADDR_ANY;
	} else {
		// address.sin_addr.s_addr =  listen.ip;
		address.sin_addr.s_addr =  htonl(listen.ip);
	}
	address.sin_port = htons(listen.port);
}

Server::Server(ConfigServer &config, int epoll_fd) : _ConfServer(config) {

	std::vector<Listen> vec_listen = this->get_listen();
	size_t size = vec_listen.size();
	struct epoll_event ev;
	struct sockaddr_in	address;
	if (size == 0){
		throw (MyException("todo faire un bind part defaut"));
	}

	for (size_t i = 0; i < size; i++)
	{
		int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd < 0){
			throw (MyException("Error : opening socket failed"));
		}
		this->vector_socket_fd.push_back(socket_fd);
		int	one = 1;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
			throw (MyException("Error : setsockopt failed ", strerror(errno)));
		}

		set_address(address, vec_listen[i]);

		if ( bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0 ){
			throw (MyException("Error : Bind failed ", strerror(errno)));
		}

		if (listen(socket_fd, 1024) < 0){
			throw (MyException("Error : Listen failed"));
		}

		set_nonblocking(socket_fd);
		// int flags = fcntl(socket_fd, F_GETFL, 0);
		// fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

		ev.events = EPOLLIN;
		ev.data.fd = socket_fd;
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);

	}
}

Server::~Server()
{
	for (size_t i = 0; i < this->vector_socket_fd.size(); i++)
	{
		close(this->vector_socket_fd[i]);
	}
	// std::cout << "destructor server " << std::endl;

}


const	std::vector<int>			&Server::get_socket_fd( void ) const { return (this->vector_socket_fd); }
const	std::vector<Listen>			&Server::get_listen( void ) const { return (this->_ConfServer.get_listen()); }
const	std::vector<std::string>	&Server::get_index( void ) const { return (this->_ConfServer.get_index()); }
const	std::vector<int>			&Server::get_error_page( void ) const { return (this->_ConfServer.get_error_page()); }
const	std::vector<std::string>	&Server::get_server_name( void ) const { return (this->_ConfServer.get_server_name()); }
const	std::vector<std::string>	&Server::get_allow_methods( void ) const { return (this->_ConfServer.get_allow_methods()); }
const	std::string					&Server::get_client_max_body_size( void ) const { return (this->_ConfServer.get_client_max_body_size()); }
const	std::string					&Server::get_root( void ) const { return (this->_ConfServer.get_root()); }


bool								Server::check_location( const std::string key ) { return (this->_ConfServer.check_location(key)); }
const	std::vector<std::string>	&Server::get_inlocation_index( void ) const { return (this->_ConfServer.get_inlocation_index());}
const	std::vector<int>			&Server::get_inlocation_error_page( void ) const { return (this->_ConfServer.get_inlocation_error_page());}
const	std::vector<std::string>	&Server::get_inlocation_allow_methods( void ) const { return (this->_ConfServer.get_inlocation_allow_methods());}
const	std::string					&Server::get_inlocation_root( void ) const { return (this->_ConfServer.get_inlocation_root());}
