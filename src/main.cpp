#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <ios>
#include <fstream>
#include "Server.hpp"
#include "Request.hpp"
#define PORT 8070
#define BUFFER_MAX 			1024
#define MAX_EVENTS 			10
#define	MAX_REQUESTS_LINE	20



// int main()
// {



// 	int sockfd;
// 	int	one = 1;
// 	struct sockaddr_in	address;
// 	socklen_t addrlen = sizeof(address);

// 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (sockfd < 0)
// 		perror("error opening socket");
// 	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

// 	address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(PORT);

// 	bind(sockfd, (struct sockaddr *)&address, addrlen);
// 	listen(sockfd, 1024);
// 	write(1, "server listening on port 8010\n", 30);
// 	int flags = fcntl(sockfd, F_GETFL, 0);
// 	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
// 	int epfd = epoll_create1(EPOLL_CLOEXEC);
// 	struct epoll_event ev;
// 	ev.events = EPOLLIN;
// 	ev.data.fd = sockfd;
// 	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
// 	while (1)
// 	{
// 		struct epoll_event events[MAX_EVENTS];
// 		int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
// 		for (int i = 0; i < nfds; i++)
// 		{
// 			if (events[i].data.fd == sockfd)
// 			{
// 				struct sockaddr_in	client_address;
// 				socklen_t	client_len = sizeof(client_address);
// 				int client_fd = accept(sockfd, (struct sockaddr *)&client_address, &client_len);
// 				flags = fcntl(client_fd, F_GETFL, 0);
// 				fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
// 				ev.events = EPOLLIN | EPOLLRDHUP;
// 				ev.data.fd = client_fd;
// 				epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
// 			}
// 			else {
// 				std::vector<char>	buffer;
// 				int client_fd = events[i].data.fd;
// 				ssize_t				bytes = 1;
// 				do {
// 					char				tmp;
// 					bytes = recv(client_fd, &tmp, sizeof(char), 0);
// 					buffer.push_back(tmp);
// 				}
// 				while (bytes > 0);
// 				if (buffer.empty())
// 					throw std::runtime_error("empty request");
// 				std::map<int, Request*> request;
// 				Request	req1(buffer);
// 				request[client_fd] = &req1;
// 				std::cout << "type: " << req1.get_type() << std::endl;
// 				write (client_fd, "HTTP/1.1 200 \r\n\r\n <html><body><h1>Hello buddy</h1></body></html>", 65);
// 				close(client_fd);
// 			}
// 		}
// 	}
// 	close(sockfd);
// 	return (0);
// }

#include "Config.hpp"
#include "Server.hpp"

int main(int ac, char **av)
{
	int epollfd = epoll_create1(EPOLL_CLOEXEC);
	if (epollfd < 0){
		std::cerr << "Error : Epoll creation failed" << std::endl;
	}


	std::vector<Server> server;

	if (ac == 2){

		try	{
			Config config(av[1]);
			config.parsingFile();

			// std::cout << config.nb_of_server() ;
			for (size_t i = 0; i < config.nb_of_server(); i++)
			{
				server.push_back(Server(config.copy_config_server(i), epollfd));
			}

		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}

	} else {
		std::cerr << "We need a Config file to lunch the server" << std::endl;
	}
	std::cout <<"\n get root " << server[1].get_root() << std::endl;
	std::cout <<"\n check perm " << server[1].check_perm("/") << std::endl;
	std::cout <<"\n check perm " << server[1].get_root() << std::endl;
}
