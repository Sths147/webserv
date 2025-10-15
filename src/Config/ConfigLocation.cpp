
#include "ConfigLocation.hpp"
#include <cstdlib> // atoi

ConfigLocation::~ConfigLocation() {}
ConfigLocation::ConfigLocation() {}

ConfigLocation::ConfigLocation( const std::string &perm , const bool &b) : _location(perm), _absolut(b), _autoindex(ON) {}



void	ConfigLocation::set_absolut( const bool &b) { this->_absolut = b; }


/* ------   _index   ------ */

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

void	ConfigLocation::set_error_page(const std::vector<std::string> &vec) {

	if (vec.size() != 2)
		throw (std::string("Error : 'error code | the page error' bad format on this line... "));
	if ( vec[0].size() > 3 )
		throw (std::string("Error : unknown error page on this line "));
	unsigned short int page = std::atoi(vec[0].c_str());
	if (vec[1][0] == '/') {
		this->_error_page[page] = vec[1];
	} else {
		throw (std::string("Error : second parameter is not a path on this line "));
	}
}

/* ------   _allow_methods   ------ */

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

void	ConfigLocation::set_root(const std::string &str) { this->_root = str; }

void	ConfigLocation::set_autoindex( const autoindexvalue b ) {this->_autoindex = b;}

void	ConfigLocation::set_return( const std::string &str ) { this->_return = str; }


void	ConfigLocation::set_cgi_path( const std::string &str ) {

	if (str.find(":") == std::string::npos) {
		throw (std::string("Error : something wrong it should be: 'extension:path'"));
	}
	std::size_t pos = str.find(":");

	if ( pos < 2 || str[pos + 1 ] == '\0' ) {
		throw (std::string("Error : something wrong it should be: 'extension:path'"));
	}
	this->_cgi_path = str;
}

void	ConfigLocation::set_cgi_extension( const std::string &str ) { this->_cgi_extension = str; }

/* ------   get inlocation   ------ */


bool													ConfigLocation::check_location( const std::string &key )	const { return (this->_location == key); }
const std::string&										ConfigLocation::get_location( void )						const { return (this->_location); }
const bool&												ConfigLocation::get_absolut( void )							const { return (this->_absolut); }
const std::vector<std::string>&							ConfigLocation::get_index( void )							const { return (this->_index); }
const std::map<unsigned short int, std::string>&		ConfigLocation::get_error_page( void )						const { return (this->_error_page); }
const std::vector<std::string>&							ConfigLocation::get_allow_methods( void )					const { return (this->_allow_methods); }
const std::string&										ConfigLocation::get_root( void )							const { return (this->_root); }
const autoindexvalue&									ConfigLocation::get_autoindex( void )						const { return (this->_autoindex); }
const std::string&										ConfigLocation::get_return( void )							const { return (this->_return); }
const std::string&										ConfigLocation::get_cgi_path( void )						const { return (this->_cgi_path); }
const std::string&										ConfigLocation::get_cgi_extension( void )					const { return (this->_cgi_extension); }
