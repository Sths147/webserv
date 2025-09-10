
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>

#include <cstring>
#include "Config.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "MyException.hpp"

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
					std::cerr << "Accept failed :" << strerror(errno) << std::endl;
					return (false);
				}

				struct sockaddr_in server_addr;
				socklen_t server_len = sizeof(server_addr);
				if (getsockname(vec_socket_fd[j], (struct sockaddr*)&server_addr, &server_len) != 0){
					std::cerr << "getsockname failed :" << strerror(errno) << std::endl;
					return (false);
				}

				bool find = false;
				Listen tmp(ntohl(client_addr.sin_addr.s_addr), ntohs(server_addr.sin_port));
				for (size_t i = 0; i < vec_listen.size(); i++)
				{
					if ((vec_listen[i].ip == 0 || vec_listen[i].ip == tmp.ip) && vec_listen[i].port == tmp.port){
						std::cout << "find : " << ntohl(client_addr.sin_addr.s_addr) << ":"<< ntohs(server_addr.sin_port) << std::endl;
						find = true;
					}
					// else {
					// 	std::cout	<< "tmp: " << ntohl(client_addr.sin_addr.s_addr) << ":"<< ntohs(server_addr.sin_port)
					// 				<< "tmp: " << tmp.ip << ":"<< tmp.port << std::endl;
					// }
				}

				if (find == false)
					return (false);
				client_socket_server[client_fd] = tmp; // save the client with the listen struct

				// Set client socket to non-blocking
				set_nonblocking(client_fd);

				// Add client socket to epoll
				ev.events = EPOLLIN | EPOLLET; // Edge-triggered
				ev.data.fd = client_fd;

				// std::cout << "New connection fd = " << client_fd << std::endl;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
					std::cerr << "Epoll add client socket failed :" << strerror(errno) << std::endl;
					close(client_fd);
					return (true);
				}
				return (true);
			}
		}
	}
	return (false);
}


Server	*find_server_from_map(Listen client_fd_info, std::vector<Server *> &vec_server, Request &req1){

	if (vec_server.size() != 1){
		for (size_t i = 0; i < vec_server.size(); i++)
		{
			if (vec_server[i]->check_listen(client_fd_info) && req1.check_hosts(vec_server[i]->get_server_name())){
				// std::cout << "server find" << std::endl;
				return (vec_server[i]);
			}
		}
	}
	if (vec_server.size() != 1){
		for (size_t i = 0; i < vec_server.size(); i++)
		{
			if (vec_server[i]->check_listen(client_fd_info)){
				// std::cout << "server find" << std::endl;
				return (vec_server[i]);
			}
		}
	}
	// std::cout << "server not find" << std::endl;
	return (vec_server[0]);
}
