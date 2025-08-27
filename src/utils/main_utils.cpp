
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "Config.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "MyException.hpp"
#include "Struct.hpp"

void set_nonblocking(int socket_fd) {
	int flags = fcntl(socket_fd, F_GETFL, 0);
	fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
}

bool	check_add_new_connection( const std::vector<Server *> &vec_server,	int &event_fd, int &epoll_fd, std::map<int, Listen> &client_socket_server){

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

Server	*find_server(Listen client_fd_info, std::vector<Server *> vec_server, Request &req1)
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
