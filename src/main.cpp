

#define MAX_EVENTS			10
#define	MAX_REQUESTS_LINE	20
#define MAX_EVENTS			10
#define MAX_BUFFER			1024

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "Config.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "MyException.hpp"



static void set_nonblocking(int socket_fd) {
	int flags = fcntl(socket_fd, F_GETFL, 0);
	fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
}

static	bool	check_add_new_connection( const std::vector<Server *> &server,	int &event_fd, int &epoll_fd){

	for (size_t i = 0; i < server.size(); i++)
	{
		std::vector<int> socket_fd = server[i]->get_socket_fd();
		for (size_t j = 0; j < socket_fd.size(); j++)
		{
			if (event_fd == socket_fd[j]){

				// std::cout << "New connection" << std::endl;
				int client_fd;
				struct epoll_event ev;

				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				client_fd = accept(socket_fd[j], (struct sockaddr*)&client_addr, &client_len);
				if (client_fd < 0) {

					perror("Accept failed");
					return (true);
				}

				// Set client socket to non-blocking
				set_nonblocking(client_fd);

				// Add client socket to epoll
				ev.events = EPOLLIN | EPOLLET; // Edge-triggered
				ev.data.fd = client_fd;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {

					perror("Epoll add client socket failed");
					close(client_fd);
					return (true);
				}
				return (true);
			}
		}
	}
	return (false);
}

int main(int ac, char **av)
{
	//----------------------------parsing of the config file + creation of every instanse of server with his config----------------------------

	int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (epoll_fd < 0){
		std::cerr << "Error : Epoll creation failed" << std::endl;
		return (1);
	}

	std::vector<Server *> server;
	if (ac == 2){

		try	{

			//First parsing of the config file to delete every empty and commentary line.
			Config config(av[1]);
			//Parsing of the config file.
			config.pars();


			for (size_t i = 0; i < config.nb_of_server(); i++)
			{
				//Creat every server with his config file added.

				// std::cout << "server n*" << i <<std::endl;
				Server *ptr = new Server(config.copy_config_server(i), epoll_fd);
				server.push_back(ptr);
			}
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			for (size_t i = 0; i < server.size(); i++)
			{
				delete server[i];
			}
			close(epoll_fd);
			return (1);
		}

	} else {
		std::cerr << "We need a Config file to lunch the server" << std::endl;
		close(epoll_fd);
		return (1);
	}

	//----------------------------main loop event epoll add a new connection read a request response to his resquest----------------------------

	std::cout << YELLOW <<"\n\tMain loop\n" << RESET << std::endl;



	int nfds;
	struct epoll_event events[MAX_EVENTS];

	while (1) {

		nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (nfds < 0) {
			std::cerr << "Epoll wait failed" << std::endl;
			for (size_t i = 0; i < server.size(); i++)
			{
				delete server[i];
			}
			return (1);
		}

		for (int i = 0; i < nfds; i++) {

			if (!check_add_new_connection(server, events[i].data.fd, epoll_fd)) {




				// Handle client data
				std::vector<char>	buffer;
				int client_fd = events[i].data.fd;
				ssize_t				bytes = 1;
				do {
					char				tmp;
					bytes = recv(client_fd, &tmp, sizeof(char), 0);
					buffer.push_back(tmp);
				}
				while (bytes > 0);
				if (buffer.empty())
					throw std::runtime_error("empty request");
				std::map<int, Request*> request;
				Request	req1(buffer);
				request[client_fd] = &req1;
				std::cout << "type: " << req1.get_type() << std::endl;
				write (client_fd, "HTTP/1.1 200 \r\n\r\n <html><body><h1>Hello buddy</h1></body></html>", 65);
				close(client_fd);




			}
		}
	}
	close(epoll_fd);
	return (0);
}


















// #include <iostream>
// #include <netinet/in.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <sys/epoll.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <map>
// #include <vector>
// #include <ios>
// #include <fstream>
// #include "Server.hpp"
// #include "Request.hpp"
// #define PORT 8070
// #define BUFFER_MAX 			1024
// #define MAX_EVENTS 			10
// #define	MAX_REQUESTS_LINE	20



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
