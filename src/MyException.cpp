
#include <iostream>

#include "MyException.hpp"

// Constructeur qui prend le message personnalisé
MyException::MyException(const std::string &msg) : _message(msg) {}
MyException::MyException(const std::string &msg1, const std::string &msg2) : _message(msg1 + msg2) {}

MyException ::~MyException() throw() {}

// what qui renvoie le message personnalisé
const char*  MyException::what() const throw()
{
    return this->_message.c_str();
}