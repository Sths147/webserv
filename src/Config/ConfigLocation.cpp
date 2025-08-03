
#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation() {}
ConfigLocation::~ConfigLocation() {}

ConfigLocation::ConfigLocation( const std::string &perm ) : _perm(perm) {}
ConfigLocation::ConfigLocation( const ConfigLocation &other ) : _perm(other._perm) {}

void	ConfigLocation::set_root(const std::string &str) { this->_root = str;}
void	ConfigLocation::set_index(const std::string &str) { this->_index.push_back(str);}
void	ConfigLocation::set_allow_methods(const std::string &str) { this->_allow_methods.push_back(str);}
void	ConfigLocation::set_error_page(const std::string &str) { this->_error_page.push_back(str);}
