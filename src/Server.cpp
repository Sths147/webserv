#include "Server.hpp"
#include "MyException.hpp"
#include <iostream>
#include <cstring>

Server::Server(void)
{
    //frist parse .conf to init port or something else.
    try
    {
        this->initSocket();
        this->bindlistensocket();
        this->_epoll_fd = epoll_create1(0);
        if (this->_epoll_fd < 0)
        {
            throw (MyException("Epoll create failed : ", strerror(errno)));
        }

        // Add server socket to epoll
        this->_ev.events = EPOLLIN;
        this->_ev.data.fd = this->_socket_fd;
        if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, this->_socket_fd, &this->_ev) < 0)
        {
            throw (MyException("Epoll add server socket failed : ", strerror(errno)));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Init server failed.\n" << e.what() << std::endl;
    }
}

void    Server::Serverloop(void)
{
    while (1)
    {
        //do 
    }
}

void Server::initSocket(void)
{
    this->_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socket_fd < 0)
    {
        throw (MyException("Socket creation failed : ", strerror(errno)));
    }

    int opt = 1;
    if (setsockopt(this->_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        throw (MyException("setsockopt failed : ", strerror(errno)));
    }
}

void Server::bindlistensocket(void)
{
    this->_server_addr.sin_family = AF_INET;
    this->_server_addr.sin_addr.s_addr = INADDR_ANY;
    this->_server_addr.sin_port = htons(PORT); // to change
    if (bind(this->_socket_fd, (struct sockaddr *)&this->_server_addr, sizeof(this->_server_addr)) < 0)
    {
        throw (MyException("bind failed : ", strerror(errno)));
    }

    if (listen(this->_socket_fd, 2) < 0)
    {
        throw (MyException("listen failed : ", strerror(errno)));
    }
    this->setnonblocking(this->_socket_fd);
}


void Server::setnonblocking(int sockfd)
{
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

Server::~Server(void) {}
