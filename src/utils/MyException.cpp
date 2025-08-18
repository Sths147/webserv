


#include <iostream>

#include "MyException.hpp"

// Constructeur qui prend le message personnalisé
MyException::MyException(const std::string &msg) : _message(RED + msg + RESET) {}
MyException::MyException(const std::string &msg1, const std::string &msg2) : _message(RED + msg1 + '"' + msg2 + '"' + RESET) {}

MyException ::~MyException() throw() {}

// what qui renvoie le message personnalisé
const char*  MyException::what() const throw()
{
	return this->_message.c_str();
}









void putRed( std::string &str ){
	std::cout << RED << str << RESET << std::endl;
}
void putGreen( std::string &str ){
	std::cout << GREEN << str << RESET << std::endl;
}
void putBlue( std::string &str ){
	std::cout << BLUE << str << RESET << std::endl;
}
void putYellow( std::string &str ){
	std::cout << YELLOW << str << RESET << std::endl;
}
