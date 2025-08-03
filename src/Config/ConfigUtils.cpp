


#include "ConfigUtils.hpp"


#define SPACE " \t\n\r\f\v"


size_t ConfigUtils::_pos = 0;




bool ConfigUtils::check_after_bracket(std::string str, size_t pos) {
	str.find_first_not_of(SPACE, pos);
	return true;
}

size_t ConfigUtils::find(std::string &str, std::string strfind){

	_pos = str.find(strfind);// find the pos of the word

	size_t pos = str.find_first_not_of(SPACE); // find the first char

	if (_pos == pos) // its the first char and the first char of the word are same that the same word
		return (_pos);
	return (std::string::npos);
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


std::string	ConfigUtils::parseDirective(const std::string& input) {
	std::string token;

	ConfigUtils::find_first_not_of_space(input);
	while (_pos < input.size()) {
		char c = input[_pos];

		if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ';' || c == '{' || c == '}')
			break;

		token += c;
		_pos++;
	}
	return token;
}
