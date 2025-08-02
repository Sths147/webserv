#include "Server.hpp"
#include "MyException.hpp"
#include <unistd.h>
#include <iostream>
#include <cstring>


Server::Server(std::string nameFile) : _conf(nameFile)
{
	this->_conf.parsingFile();

}

Server::~Server()
{

}
