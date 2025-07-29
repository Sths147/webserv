#include "Server.hpp"
#include "MyException.hpp"
#include <unistd.h>
#include <iostream>
#include <cstring>


Server::Server(std::string nameFile) : _conf(nameFile)
{

}

Server::~Server()
{

}
