/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcretin <fcretin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 14:11:35 by fcretin           #+#    #+#             */
/*   Updated: 2025/08/27 11:20:49 by fcretin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "ConfigServer.hpp"
#include "ConfigLocation.hpp"
#include "MyException.hpp"

ConfigServer::ConfigServer() : _last_i_location(-1) {}
ConfigServer::~ConfigServer() {}
// ConfigServer::ConfigServer() {std::cout << "\ncontruct\n";}
// ConfigServer::~ConfigServer() { std::cout << "\ndestru\n";}


#include <cstdlib> // atoi


/* ------   _listen   ------ */

void	ConfigServer::print_listen( void ){
	std::cout << "\nlisten :"<< std::endl;
	for (size_t i = 0; i < this->_listen.size(); i++)
	{
		std::cout	<< "ip:'" << this->_listen[i].ip << "'\t"
					<< "port:'" << this->_listen[i].port << "'" << std::endl;
	}
}


void	ConfigServer::set_listen( const std::string &str)
{
	this->_listen.push_back(ConfigUtils::ip_host_parseur(str));//todo stocker
}


/* ------   _index   ------ */

void	ConfigServer::print_index( void ){
	std::cout << "\nindex :"<< std::endl;
	for (size_t i = 0; i < this->_index.size(); i++)
	{
		std::cout << "'" << this->_index[i] << "'" << std::endl;
	}
}
void	ConfigServer::set_index( const std::vector<std::string> vec ){
	if (this->_index.size() == 0)
		this->_index = vec;
	else	{
		for (size_t i = 0; i < vec.size(); i++)
		{
			this->_index.push_back(vec[i]);
		}
	}
}


/* ------   _error_page   ------ */


void	ConfigServer::print_error_page( void ){
	std::cout << "\nerror_page :"<< std::endl;
	for (size_t i = 0; i < this->_error_page.size(); i++)
	{
		std::cout << "'" << this->_error_page[i] << "'" << std::endl;
	}
}
void	ConfigServer::set_error_page( const std::vector<std::string> vec ){

	if (vec.size() != 2)
		throw (std::string("Error : number error | the page error format on this line "));
	if ( vec[0].size() > 3 )
		throw (std::string("Error : unknown error page on this line "));
	unsigned short int page = std::atoi(vec[0].c_str());
	if (ConfigUtils::error_page_valid(page))
		this->_error_page[page] = vec[1];
	else
		throw (std::string("Error : unused error number page on this line "));

}


/* ------   _server_name   ------ */


void	ConfigServer::print_server_name( void ){
	std::cout << "\nserver_name :"<< std::endl;
	for (size_t i = 0; i < this->_server_name.size(); i++)
	{
		std::cout << "'" << this->_server_name[i] << "'" << std::endl;
	}
}
void	ConfigServer::set_server_name( const std::vector<std::string> vec ) {
	if (this->_server_name.size() == 0)
		this->_server_name = vec;
	else {
		for (size_t i = 0; i < vec.size(); i++)
		{
			this->_server_name.push_back(vec[i]);
		}
	}
}


/* ------   _allow_methods   ------ */

void	ConfigServer::print_allow_methods( void ){
	std::cout << "\nallow_methods :"<< std::endl;
	for (size_t i = 0; i < this->_allow_methods.size(); i++)
	{
		std::cout << "'" << this->_allow_methods[i] << "'" << std::endl;
	}
}
void	ConfigServer::set_allow_methods( const std::vector<std::string> vec ){
		if (this->_allow_methods.size() == 0)
		this->_allow_methods = vec;
	else {
		for (size_t i = 0; i < vec.size(); i++)
		{
			this->_allow_methods.push_back(vec[i]);
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



/* ------   location   ------ */

void	ConfigServer::print_location( void ){

	for (size_t i = 0; i < this->_vConfLocal.size(); i++)
	{
		std::cout << YELLOW << "\tlocation n" << i << RESET << std::endl;
		this->_vConfLocal[i].print_all();
	}


}
void	ConfigServer::set_new_location( const std::string &perm, const bool &b) {
	this->_vConfLocal.push_back(ConfigLocation(perm , b));
}
/* --- set in vector location index--- */
void	ConfigServer::set_inlocation_index(const int &i, const std::vector<std::string> &arg){this->_vConfLocal[i].set_index(arg);}
void	ConfigServer::set_inlocation_allow_methods(const int &i, const std::vector<std::string> &arg){this->_vConfLocal[i].set_allow_methods(arg);}
void	ConfigServer::set_inlocation_error_page(const int &i, const std::vector<std::string> &arg){this->_vConfLocal[i].set_error_page(arg);}
void	ConfigServer::set_inlocation_root(const int &i, const std::string &str){this->_vConfLocal[i].set_root(str);}




/* --- GET --- */

const	std::vector<Listen>							&ConfigServer::get_listen( void ) const { return (this->_listen);}
const	std::vector<std::string>					&ConfigServer::get_index( void ) const { return (this->_index);}
const	std::map<unsigned short int, std::string>	&ConfigServer::get_error_page( void ) const { return (this->_error_page);}
const	std::vector<std::string>					&ConfigServer::get_server_name( void ) const { return (this->_server_name);}
const	std::vector<std::string>					&ConfigServer::get_allow_methods( void ) const { return (this->_allow_methods);}
const	std::string									&ConfigServer::get_client_max_body_size( void ) const { return (this->_client_max_body_size);}
const	std::string									&ConfigServer::get_root( void ) const { return (this->_root);}



/* --- GET inlocation --- */

bool						ConfigServer::check_location( std::string key )
{
	this->_last_i_location = -1;
	for (size_t i = 0; i < this->_vConfLocal.size(); i++)
	{
		if (this->_vConfLocal[i].check_location(key) && this->_vConfLocal[i].get_absolut()){
			this->_last_i_location = i;
			return (true);
		}
	}
	do
	{
		for (size_t i = 0; i < this->_vConfLocal.size() ; i++)
		{
			// std::cout << key <<std::endl;
			if (this->_vConfLocal[i].check_location(key) && !this->_vConfLocal[i].get_absolut()) {
				this->_last_i_location = i;
				return (true);
			}
		}
		size_t pos = key.find_last_of('/');
		if (pos == std::string::npos || key.size() == 1)
			break;
		if (pos == 0) {
			key.resize(1);
		} else {
			key.resize(pos);
		}
	} while (1);
	return (false);
}

const	std::string									&ConfigServer::get_inlocation_location( void ) const { if (this->_last_i_location == -1) throw (MyException("Error : no location find vector index == -1" , "contact moi si tu a ce msg")); return (this->_vConfLocal[this->_last_i_location].get_location()); }

const	std::vector<std::string>					&ConfigServer::get_inlocation_index( void ) const { if (this->_last_i_location == -1) throw (MyException("Error : no location find vector index == -1" , "contact moi si tu a ce msg")); return (this->_vConfLocal[this->_last_i_location].get_index()); }

const	std::map<unsigned short int, std::string>	&ConfigServer::get_inlocation_error_page( void ) const { if (this->_last_i_location == -1) throw (MyException("Error : no location find vector index == -1" , "contact moi si tu a ce msg")); return (this->_vConfLocal[this->_last_i_location].get_error_page()); }

const	std::vector<std::string>					&ConfigServer::get_inlocation_allow_methods( void ) const { if (this->_last_i_location == -1) throw (MyException("Error : no location find vector index == -1" , "contact moi si tu a ce msg")); return (this->_vConfLocal[this->_last_i_location].get_allow_methods()); }

const	std::string									&ConfigServer::get_inlocation_root( void ) const { if (this->_last_i_location == -1) throw (MyException("Error : no location find vector index == -1" , "contact moi si tu a ce msg")); return (this->_vConfLocal[this->_last_i_location].get_root()); }

