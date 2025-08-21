#include "Server.hpp"
#include "MyException.hpp"
#include <unistd.h>
#include <iostream>
#include <cstring>

Server::Server( void )
{

}

// static void set_nonblocking(int socket_fd) {
// 	int flags = fcntl(socket_fd, F_GETFL, 0);
// 	fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
// }

// static void set_address(struct sockaddr_in	&address, Listen &listen) {
// 	address.sin_family = AF_INET;
// 	address.sin_addr.s_addr = listen.ip;
// 	address.sin_port = htons(listen.port);
// }

Server::Server(ConfigServer &config, int epoll_fd) : _ConfServer(config) {

	(void) epoll_fd;
	// std::vector<Listen> vec_listen = this->get_listen();
	// if (vec_listen.size() == 0){

	// 	throw (MyException("todo faire un bind part defaut"));

	// }

	// int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	// if (socket_fd < 0){
	// 	throw (MyException("Error : opening socket failed"));
	// }

	// int	one = 1;
	// if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
	// 	throw (MyException("Error : setsockopt failed ", strerror(errno)));
	// }

	// struct sockaddr_in	address;
	// set_address(set_address, vec_listen[i];)

	// if ( bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0 ){
	// 	throw (MyException("Error : Bind failed"));
	// }

	// if (listen(socket_fd, 1024) < 0){

	// 	throw (MyException("Error : Listen failed"));

	// }

	// set_nonblocking(socket_fd);

	// // std::cout <<  "server listening on port" << ,
	// // write(1, "server listening on port 8010\n", 30);
	// // int flags = fcntl(socket_fd, F_GETFL, 0);
	// // fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);


	// // ??
	// struct epoll_event ev;
	// ev.events = EPOLLIN;
	// ev.data.fd = socket_fd;
	// epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);

}

Server::~Server()
{

}


const	std::vector<Listen>			&Server::get_listen( void ) const { return (this->_ConfServer.get_listen()); }
const	std::vector<std::string>	&Server::get_index( void ) const { return (this->_ConfServer.get_index()); }
const	std::vector<int>			&Server::get_error_page( void ) const { return (this->_ConfServer.get_error_page()); }
const	std::vector<std::string>	&Server::get_server_name( void ) const { return (this->_ConfServer.get_server_name()); }
const	std::vector<std::string>	&Server::get_allow_methods( void ) const { return (this->_ConfServer.get_allow_methods()); }
const	std::string					&Server::get_client_max_body_size( void ) const { return (this->_ConfServer.get_client_max_body_size()); }
const	std::string					&Server::get_root( void ) const { return (this->_ConfServer.get_root()); }


bool								Server::check_perm( const std::string key ) { return (this->_ConfServer.check_perm(key)); }
const	std::vector<std::string>	&Server::get_inlocation_index( void ) const { return (this->_ConfServer.get_inlocation_index());}
const	std::vector<int>			&Server::get_inlocation_error_page( void ) const { return (this->_ConfServer.get_inlocation_error_page());}
const	std::vector<std::string>	&Server::get_inlocation_allow_methods( void ) const { return (this->_ConfServer.get_inlocation_allow_methods());}
const	std::string					&Server::get_inlocation_root( void ) const { return (this->_ConfServer.get_inlocation_root());}
