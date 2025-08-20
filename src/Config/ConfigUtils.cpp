
#include "ConfigUtils.hpp"
#include "MyException.hpp"


#define SPACE " \t\n\r\f\v"

size_t ConfigUtils::_pos = 0;

bool ConfigUtils::error_page_valid(const int &page){
	return (page == 404 || page == 500|| page == 502 || page == 503 || page == 504);
}
void ConfigUtils::check_after_bracket_semicolon(const std::string &str, size_t pos) {
	if (ConfigUtils::find_first_not_of_space(str, pos) != std::string::npos) {
		if (str[_pos] == '#')
			return ;
		throw (MyException("2 Error : extra charater... ", str));
	}
	return  ;
}

void ConfigUtils::check_bracket(const std::string &str) {
	if (ConfigUtils::find_first_not_of_space(str) != std::string::npos && str[ConfigUtils::get_pos()] == '}'){
		ConfigUtils::check_after_bracket_semicolon(str, ConfigUtils::get_pos() + 1);
	} else {
		throw (MyException("Error : unknown directive...", str));
	}
}


size_t ConfigUtils::find_first_not_of_space(const std::string &str) {
	_pos = str.find_first_not_of(SPACE);
	return (_pos);
}
size_t ConfigUtils::find_first_not_of_space(const std::string &str, size_t pos) {
	_pos = str.find_first_not_of(SPACE, pos);
	return (_pos);
}
size_t ConfigUtils::get_pos( void ) {
	return (_pos);
}


std::string	ConfigUtils::parse_token(const std::string& input, const size_t pos) {
	std::string token;

	ConfigUtils::find_first_not_of_space(input, pos);
	while (_pos < input.size()) {
		char c = input[_pos];

		if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ';' || c == '{' || c == '}')
			break;

		token += c;
		_pos++;
	}
	return (token);
}

std::vector<std::string>	ConfigUtils::parse_multi_token(const std::string& input, const size_t pos) {
	std::vector<std::string> vec;
	std::string token;
	char c;

	ConfigUtils::find_first_not_of_space(input, pos);
	while (_pos < input.size()) {
		while (_pos < input.size()) {
			c = input[_pos];
			if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ';' || c == '{' || c == '}')
				break;
			token += c;
			_pos++;
		}
		ConfigUtils::find_first_not_of_space(input, _pos);
		if (token != "\0"){
			vec.push_back(token);
			token.clear();
		}
		if (c == ';')
			break;
	}
	return (vec);
}

std::string	ConfigUtils::get_one_token(const std::string &str) {
	std::string arg = ConfigUtils::parse_token(str, _pos);
	if (_pos == str.find_first_of(';'))
		ConfigUtils::check_after_bracket_semicolon(str, _pos + 1);
	return (arg);
}


std::vector<std::string>	ConfigUtils::get_multi_token(const std::string &str) {
	std::vector<std::string> vec = ConfigUtils::parse_multi_token(str, _pos);
	if (_pos == str.find_first_of(';'))
		ConfigUtils::check_after_bracket_semicolon(str, _pos + 1);
	return (vec);
}

#include <sstream> // pour stringstream
std::vector<std::string> ConfigUtils::split(const std::string &topars, char delim)
{
	std::vector<std::string> tokens;
	std::stringstream ss(topars.c_str());
	std::string str;

	while (std::getline(ss, str, delim)) {
		tokens.push_back(str);
	}
	return (tokens);
}
