
#include "ConfigServer.hpp"
#include "ConfigLocation.hpp"
#include "MyException.hpp"

ConfigServer::ConfigServer() {}
ConfigServer::ConfigServer( const ConfigServer &other) {(void)other;}
ConfigServer::~ConfigServer() {}

#include <cstdlib>



/* ------   _listen   ------ */

void	ConfigServer::print_listen( void ){
	std::cout << "\nlisten :"<< std::endl;
	for (size_t i = 0; i < this->_listen.size(); i++)
	{
		std::cout << "'" << this->_listen[i] << "'" << std::endl;
	}
}
void	ConfigServer::set_listen( const std::string &str)
{
	this->_listen.push_back(str);//todo stocker
}



/* ------   _server_name   ------ */

void	ConfigServer::print_server_name( void ){
	std::cout << "\nserver_name :"<< std::endl;
	for (size_t i = 0; i < this->_server_name.size(); i++)
	{
		std::cout << "'" << this->_server_name[i] << "'" << std::endl;
	}
}
void	ConfigServer::set_server_name( const std::vector<std::string> &vec ) {
	if (this->_server_name.size() == 0)
		this->_server_name = vec;
	else {
		for (size_t i = 0; i < vec.size(); i++)
		{
			this->_server_name.push_back(vec[i]);
		}
	}
}



/* ------   _client_max_body_size   ------ */

void	ConfigServer::print_client_max_body_size( void ){
	std::cout << "\nclient_max_body_size :"<< std::endl;
	if (this->_client_max_body_size != "\0"){
		std::cout << "'" << this->_client_max_body_size << "'" << std::endl;
	}
}
void	ConfigServer::set_client_max_body_size( const std::string &str){ this->_client_max_body_size = str;} // convert



/* ------   _root   ------ */

void	ConfigServer::print_root( void ){
	std::cout << "\nroot :"<< std::endl;
	if (this->_root != "\0"){
		std::cout << "'" << this->_root << "'" << std::endl;
	}
}
void	ConfigServer::set_root( const std::string &str){ this->_root = str;}



/* ------   _index   ------ */

void	ConfigServer::print_index( void ){
	std::cout << "\nindex :"<< std::endl;
	for (size_t i = 0; i < this->_index.size(); i++)
	{
		std::cout << "'" << this->_index[i] << "'" << std::endl;
	}
}
void	ConfigServer::set_index( const std::string &str){ this->_index.push_back(str);}



/* ------   _allow_methods   ------ */

void	ConfigServer::print_allow_methods( void ){
	std::cout << "\nallow_methods :"<< std::endl;
	for (size_t i = 0; i < this->_allow_methods.size(); i++)
	{
		std::cout << "'" << this->_allow_methods[i] << "'" << std::endl;
	}
}
void	ConfigServer::set_allow_methods( const std::string &str){ this->_allow_methods.push_back(str);}



/* ------   _error_page   ------ */

void	ConfigServer::print_error_page( void ){
	std::cout << "\nerror_page :"<< std::endl;
	for (size_t i = 0; i < this->_error_page.size(); i++)
	{
		std::cout << "'" << this->_error_page[i] << "'" << std::endl;
	}
}
void	ConfigServer::set_error_page( const std::string &str){ this->_error_page.push_back(str);}



/* ------   location   ------ */

void	ConfigServer::set_new_location( const std::string &str) {
	this->_vConfLocaP.push_back(ConfigLocation(str));

}
/* --- set in vector location index--- */
void	ConfigServer::set_inlocation_root(const int &i, const std::string &str){this->_vConfLocaP[i].set_root(str);}
void	ConfigServer::set_inlocation_index(const int &i, const std::string &str){this->_vConfLocaP[i].set_index(str);}
void	ConfigServer::set_inlocation_allow_methods(const int &i, const std::string &str){this->_vConfLocaP[i].set_allow_methods(str);}
void	ConfigServer::set_inlocation_error_page(const int &i, const std::string &str){this->_vConfLocaP[i].set_error_page(str);}
