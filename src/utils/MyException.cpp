


#include <iostream>

#include "MyException.hpp"

MyException ::~MyException() throw() {}
MyException::MyException(const std::string &msg) : _message(RED + msg + RESET) {}
MyException::MyException(const std::string &msg1, const std::string &msg2) : _message(RED + msg1 + '"' + msg2 + '"' + RESET) {}


const char*  MyException::what() const throw()
{
	return this->_message.c_str();
}
