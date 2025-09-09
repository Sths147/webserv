
#include "ConfigUtils.hpp"
#include "MyException.hpp"


#define SPACE " \t\n\r\f\v"

size_t ConfigUtils::_pos = 0;

void ConfigUtils::check_after_bracket_semicolon(const std::string &str, size_t pos) {
	if (ConfigUtils::find_first_not_of_space(str, pos) != std::string::npos) {
		if (str[_pos] == '#')
			return ;
		throw (MyException("Error : extra charater... ", str));
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

#include <cstdlib> // atoi
static unsigned int	ipconvert(std::string& str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		char c = str[i];
		if (!(std::isdigit(c) || c == '.'))
			throw (std::string("Error : invalid character on IP of this line "));
	}

	std::vector<std::string> vecstring = ConfigUtils::split(str, '.');
	if (vecstring.size() != 4 || vecstring[0] == "\0" || vecstring[1] == "\0" || vecstring[2] == "\0" || vecstring[3] == "\0"
		|| vecstring[0].size() > 3|| vecstring[1].size() > 3|| vecstring[2].size() > 3|| vecstring[3].size() > 3)
		throw (std::string("Error : bad format on IP of this line "));

	// std::cout << "\na = " << a << ", b = " << b << ", c = " << c << ", d = " << d <<std::endl;

	unsigned int	a = std::atoi(vecstring[0].c_str());
	unsigned int	b = std::atoi(vecstring[1].c_str());
	unsigned int	c = std::atoi(vecstring[2].c_str());
	unsigned int	d = std::atoi(vecstring[3].c_str());

	if (a > 255 || b > 255 || c > 255 || d > 255)
		throw std::string("Error: invalid IP part (must be between 0 and 255)");

	return ((a << 24) | (b << 16) | (c << 8) | d);
}

Listen	ConfigUtils::ip_host_parseur( const std::string &str)
{

	unsigned int ip = 0, port = 8080;
	if (str.find_first_of(':') == std::string::npos && !str.empty()) {
		for (size_t i = 0; i < str.size(); i++)
			if (!std::isdigit(str[i]))
				throw(std::string("Error : the port on this line doesnt have onlydigit"));
		port = std::atoi(str.c_str());
		return (Listen(ip, port));
	}

	if (str.find_first_of(':') != str.find_last_of(':')) {
		throw (std::string("Error : Multi ':' on this line ")); // listen :80:;
	}
	std::vector<std::string> vecstring = ConfigUtils::split(str, ':');

	if (vecstring.size() == 0 || (vecstring.size() == 1 && vecstring[0] == "\0"))
		throw (std::string("Error : No value on this line ")); // listen :;



	if (vecstring[0] != "\0"){
		// std::cout << "\tip = " << vecstring[0];
		if (vecstring[0] == "localhost")
			ip = 2130706433;
		else
			ip = ipconvert(vecstring[0]);
	}
	if (vecstring.size() == 2 &&vecstring[1] != "\0"){
		// std::cout << "\tport = " << vecstring[1];
		for (size_t i = 0; i < vecstring[1].size(); i++)
			if (!std::isdigit(vecstring[1][i]))
				throw(std::string("Error : the port on this line doesnt have onlydigit "));
		port = std::atoi(vecstring[1].c_str());
	}
	return (Listen(ip, port));
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
