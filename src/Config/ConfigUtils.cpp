


#include "ConfigUtils.hpp"
#include "MyException.hpp"


#define SPACE " \t\n\r\f\v"

size_t ConfigUtils::_pos = 0;

void ConfigUtils::check_after_bracket_semicolon(const std::string &str, size_t pos) {
	if (ConfigUtils::find_first_not_of_space(str, pos) != std::string::npos) {
		if (str[_pos] == '#')
			return ;
		throw (MyException("2 Error : extra charater... ", str));
	}
	return  ;
}

void ConfigUtils::check_bracket(const std::string &str){
	if (ConfigUtils::find_first_not_of_space(str) != std::string::npos && str[ConfigUtils::get_pos()] == '}'){
		ConfigUtils::check_after_bracket_semicolon(str, ConfigUtils::get_pos() + 1);
	} else {
		throw (MyException("Error : unknown directive...", str));
	}
}


size_t ConfigUtils::find_first_not_of_space(const std::string &str){
	_pos = str.find_first_not_of(SPACE);
	return (_pos);
}
size_t ConfigUtils::find_first_not_of_space(const std::string &str, size_t pos){
	_pos = str.find_first_not_of(SPACE, pos);
	return (_pos);
}
size_t ConfigUtils::get_pos( void ) {
	return (_pos);
}


std::string	ConfigUtils::parseToken(const std::string& input, const size_t pos) {
	std::string token;

	ConfigUtils::find_first_not_of_space(input, pos);
	while (_pos < input.size()) {
		char c = input[_pos];

		if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ';' || c == '{' || c == '}')
			break;

		token += c;
		_pos++;
	}
	return token;
}
