#include "Server.hpp"
#include "MyException.hpp"
#include <unistd.h>
#include <iostream>
#include <cstring>

Server::Server( void )
{

}

Server::Server(ConfigServer &config) : _ConfServer(config) {}

Server::~Server()
{

}


std::vector<Listen>			&Server::get_listen( void ) { return (this->_ConfServer.get_listen()); }
std::vector<std::string>	&Server::get_index( void ) { return (this->_ConfServer.get_index()); }
std::vector<int>			&Server::get_error_page( void ) { return (this->_ConfServer.get_error_page()); }
std::vector<std::string>	&Server::get_server_name( void ) { return (this->_ConfServer.get_server_name()); }
std::vector<std::string>	&Server::get_allow_methods( void ) { return (this->_ConfServer.get_allow_methods()); }
std::string					&Server::get_client_max_body_size( void ) { return (this->_ConfServer.get_client_max_body_size()); }
std::string					&Server::get_root( void ) { return (this->_ConfServer.get_root()); }
