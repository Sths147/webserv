

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
#include "MyException.hpp"
#include "Struct.hpp"
#include "Response.hpp"
#include <map>


static void set_nonblocking(int socket_fd) {
	int flags = fcntl(socket_fd, F_GETFL, 0);
	fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
}

static	bool	check_add_new_connection( const std::vector<Server *> &vec_server,	int &event_fd, int &epoll_fd, std::map<int, Listen> &client_socket_server){

	for (size_t i = 0; i < vec_server.size(); i++)
	{
		std::vector<int> vec_socket_fd = vec_server[i]->get_socket_fd();
		const std::vector<Listen> &vec_listen = vec_server[i]->get_listen();
		for (size_t j = 0; j < vec_socket_fd.size(); j++)
		{
			if (event_fd == vec_socket_fd[j]){

				int client_fd;
				struct epoll_event ev;
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				client_fd = accept(vec_socket_fd[j], (struct sockaddr*)&client_addr, &client_len);
				if (client_fd < 0) {

					perror("Accept failed");
					return (true);
				}
				// todo print i / j find server will find the same one and return the server
				client_socket_server[client_fd] = vec_listen[j];

				// Set client socket to non-blocking
				set_nonblocking(client_fd);

				// Add client socket to epoll
				ev.events = EPOLLIN | EPOLLET; // Edge-triggered
				ev.data.fd = client_fd;

				// std::cout << "New connection fd = " << client_fd << std::endl;
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

static Server	*find_server(Listen client_fd_info, std::vector<Server *> vec_server, Request &req1)
{
	Server *ptr = NULL;
	bool first = false;
	(void)req1;
	for (size_t i = 0; i < vec_server.size(); i++)
	{
		std::vector<Listen> vec_listen = vec_server[i]->get_listen();
		for (size_t j = 0; j < vec_listen.size(); j++)
		{
			if (!first && vec_listen[j].ip == client_fd_info.ip && vec_listen[j].port == client_fd_info.port) {
				ptr = vec_server[i];
			} else if (vec_listen[j].ip == client_fd_info.ip && vec_listen[j].port == client_fd_info.port && req1.check_hosts(vec_server[i]->get_server_name())){
				ptr = vec_server[i];
			}
		}
	}
	return (ptr);
}

int main(int ac, char **av)
{
	//----------------------------parsing of the config file + creation of every instanse of server with his config----------------------------

	int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (epoll_fd < 0){
		std::cerr << "Error : Epoll creation failed" << std::endl;
		return (1);
	}

	std::vector<Server *> vec_server;
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
				vec_server.push_back(ptr);
			}
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			for (size_t i = 0; i < vec_server.size(); i++)
			{
				delete vec_server[i];
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
	std::map<int, Listen> client_socket_server;
	while (1) {

		nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (nfds < 0) {
			std::cerr << "Epoll wait failed" << std::endl;
			for (size_t i = 0; i < vec_server.size(); i++)
			{
				delete vec_server[i];
			}
			return (1);
		}

		for (int i = 0; i < nfds; i++) {

			if (!check_add_new_connection(vec_server, events[i].data.fd, epoll_fd, client_socket_server)) {

				// Handle client data
				std::vector<char>	buffer;
				int client_fd = events[i].data.fd;
				// std::cout << "request fd = " << client_fd << std::endl;

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
				Server *serv = find_server(client_socket_server[client_fd], vec_server, req1);
				// std::cout << &serv << std::endl;
				// (void)serv;
				Response rep(req1, *serv);
				rep.write_response(client_fd);
				std::cout << "type: " << req1.get_type() << std::endl;
				// write (client_fd, "HTTP/1.1 200 \r\n\r\n <html><body><h1>Hello buddy</h1></body></html>", 65);
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
