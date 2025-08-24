
#include "ConfigLocation.hpp"
#include <cstdlib> // atoi

ConfigLocation::ConfigLocation() {}
ConfigLocation::~ConfigLocation() {}

ConfigLocation::ConfigLocation( const std::string &perm , const bool &b) : _location(perm), _absolut(b) {}
ConfigLocation::ConfigLocation( const std::string &perm ) : _location(perm) {}






void	ConfigLocation::print_all( void ){
	std::cout << "location : " <<this->_location << std::endl;
	std::cout << "\nabsolut : " <<this->_absolut << std::endl;
	this->print_index();
	this->print_error_page();
	this->print_allow_methods();
	this->print_root();
}

void	ConfigLocation::set_absolut( const bool &b) {this->_absolut = b;}

/* ------   _index   ------ */

void	ConfigLocation::print_index( void ){
	std::cout << "\nindex :"<< std::endl;
	for (size_t i = 0; i < this->_index.size(); i++)
	{
		std::cout << "'" << this->_index[i] << "'" << std::endl;
	}
}
void	ConfigLocation::set_index(const std::vector<std::string> &arg) {
	if (this->_index.size() == 0)
		this->_index = arg;
	else {
		for (size_t i = 0; i < arg.size(); i++)
		{
			this->_index.push_back(arg[i]);
		}
	}
}

/* ------   _error_page   ------ */

void	ConfigLocation::print_error_page( void ){
	std::cout << "\nerror_page :"<< std::endl;
	for (size_t i = 0; i < this->_error_page.size(); i++)
	{
		std::cout << "'" << this->_error_page[i] << "'" << std::endl;
	}
}

void	ConfigLocation::set_error_page(const std::vector<std::string> &arg) {

	for (size_t i = 0; i < arg.size(); i++)
	{
		if (arg[i].size() > 3)
			throw (std::string("Error : unknown error page on this line "));
		int page = std::atoi(arg[i].c_str());
		if (ConfigUtils::error_page_valid(page))
			this->_error_page.push_back(page);
		else
			throw (std::string("Error : unknown error page on this line "));
	}


}

/* ------   _allow_methods   ------ */

void	ConfigLocation::print_allow_methods( void ){
	std::cout << "\nallow_methods :"<< std::endl;
	for (size_t i = 0; i < this->_allow_methods.size(); i++)
	{
		std::cout << "'" << this->_allow_methods[i] << "'" << std::endl;
	}
}
void	ConfigLocation::set_allow_methods(const std::vector<std::string> &arg) {
	if (this->_allow_methods.size() == 0) {
		this->_allow_methods = arg;
	} else {
		for (size_t i = 0; i < arg.size(); i++)
		{
			this->_allow_methods.push_back(arg[i]);
		}
	}
}

/* ------   _root   ------ */

void	ConfigLocation::print_root( void ){
	std::cout << "\nroot :"<< std::endl;
	if (this->_root != "\0"){
		std::cout << "'" << this->_root << "'" << std::endl;
	}
}

void	ConfigLocation::set_root(const std::string &str) { this->_root = str;}















bool							ConfigLocation::check_location( const std::string &key ) const { return (this->_location == key); }

const bool						&ConfigLocation::get_absolut( void ) const { return (this->_absolut);}

const std::string				&ConfigLocation::get_location( void ) const { return (this->_location);}

const std::vector<std::string>	&ConfigLocation::get_index( void ) const { return (this->_index);}

const std::vector<int>			&ConfigLocation::get_error_page( void ) const { return (this->_error_page);}

const std::vector<std::string>	&ConfigLocation::get_allow_methods( void ) const { return (this->_allow_methods);}

const std::string				&ConfigLocation::get_root( void ) const { return (this->_root);}

