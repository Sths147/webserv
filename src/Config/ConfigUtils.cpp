#include "ConfigUtils.hpp"

size_t ConfigUtils::_pos = 0;
// int	ConfigUtils::number_of_bracket()
// {

// }

bool ConfigUtils::check_after_bracket(std::string str, size_t pos) {
	str.find_first_not_of(" \t\n\r\f\v", pos);
	return true;
}

size_t ConfigUtils::find(std::string &str, std::string strfind){
	_pos = str.find(strfind);
	return (_pos);
}

size_t ConfigUtils::get_pos( void ) {
	return (_pos);
}
