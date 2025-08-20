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
