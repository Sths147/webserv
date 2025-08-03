
#include "ConfigServer.hpp"
#include "ConfigLocation.hpp"

ConfigServer::ConfigServer() {}
ConfigServer::ConfigServer( const ConfigServer &other) {(void)other;}
ConfigServer::~ConfigServer() {}

/* --- set this class--- */
void	ConfigServer::set_listen( const std::string &str){ this->_listen = str;}
void	ConfigServer::set_host( const std::string &str){ this->_host = str;}
void	ConfigServer::set_server_name( const std::string &str){ this->_server_name = str;}
void	ConfigServer::set_client_max_body_size( const std::string &str){ this->_client_max_body_size = str;}
void	ConfigServer::set_root( const std::string &str){ this->_root = str;}
void	ConfigServer::set_index( const std::string &str){ this->_index.push_back(str);}
void	ConfigServer::set_allow_methods( const std::string &str){ this->_allow_methods.push_back(str);}
void	ConfigServer::set_error_page( const std::string &str){ this->_error_page.push_back(str);}


void	ConfigServer::set_new_location( const std::string &str) {
	this->_vConfLocaP.push_back(ConfigLocation(str));

}


/* --- set in vector location index--- */
void	ConfigServer::set_inlocation_root(const int &i, const std::string &str){this->_vConfLocaP[i].set_root(str);}
void	ConfigServer::set_inlocation_index(const int &i, const std::string &str){this->_vConfLocaP[i].set_index(str);}
void	ConfigServer::set_inlocation_allow_methods(const int &i, const std::string &str){this->_vConfLocaP[i].set_allow_methods(str);}
void	ConfigServer::set_inlocation_error_page(const int &i, const std::string &str){this->_vConfLocaP[i].set_error_page(str);}
