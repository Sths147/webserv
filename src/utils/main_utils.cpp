
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>

#include <cstring>
#include "Config.hpp"
#include "Server.hpp"
#include "MyException.hpp"
#include "ClientFd.hpp"


bool epollctl(int epoll_fd, int client_fd, const int events, int op){

	struct epoll_event ev;
	std::memset(&ev, 0, sizeof(ev));

	ev.events = EPOLLRDHUP | events ;// EPOLLIN : EPOLLOUT
	ev.data.fd = client_fd;

	// op = EPOLL_CTL_ADD : EPOLL_CTL_MOD
	if (epoll_ctl(epoll_fd, op, client_fd, &ev) < 0) {
		std::cerr << "epoll_ctl failed :" << strerror(errno) << std::endl;
		return (false);
	}
	return true;
}

void set_nonblocking(int socket_fd) {
	int flags = fcntl(socket_fd, F_GETFL, 0);
	fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
}

bool check_add_new_connection(const std::vector<Server *> &vec_server, int &event_fd, int &epoll_fd, std::map<int, ClientFd> &client_socket_server)
{
	// std::cout << "Debug check_add_new_connection" << std::endl;
	for (size_t i = 0; i < vec_server.size(); i++)
	{
		std::vector<int> vec_socket_fd = vec_server[i]->get_socket_fd();
		const std::vector<Listen> &vec_listen = vec_server[i]->get_listen();

		for (size_t j = 0; j < vec_socket_fd.size(); j++)
		{
			if (event_fd == vec_socket_fd[j]){

				int client_fd;
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				client_fd = accept(vec_socket_fd[j], (struct sockaddr*)&client_addr, &client_len);
				if (client_fd < 0) {
					std::cerr << "accept failed :" << strerror(errno) << std::endl;
					return (false);
				}


				struct linger sl;
				sl.l_onoff = 1;  // option on
				sl.l_linger = 0; // delai a 0s
				if (setsockopt(client_fd, SOL_SOCKET, SO_LINGER, &sl, sizeof(sl)) < 0) { // l'envoie du paquet rst
					std::cerr << "setsockopt failed: " << strerror(errno) << std::endl;
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
						find = true;
					}
					// else {
					// 	std::cout	<< "tmp: " << ntohl(client_addr.sin_addr.s_addr) << ":"<< ntohs(server_addr.sin_port)
					// 				<< "tmp: " << tmp.ip << ":"<< tmp.port << std::endl;
					// }
				}
				if (find == false)
					return (false);


				// Set client socket to non-blocking
				set_nonblocking(client_fd);
				if (!epollctl(epoll_fd, client_fd, EPOLLIN, EPOLL_CTL_ADD)) {
					close(client_fd);
					return (true);
				}
				client_socket_server[client_fd] = ClientFd(client_fd, tmp); // save the client with the listen struct
				std::cout	<< GREEN << "add new connection " << RESET << client_fd << std::endl;
				return (true);
			}
		}
	}
	return (false);
}

Server	*find_server_from_map(Listen client_fd_info, std::vector<Server *> &vec_server, Request &req1){

	for (size_t i = 0; i < vec_server.size(); i++) {
		if (vec_server[i]->check_listen(client_fd_info) && req1.check_hosts(vec_server[i]->get_server_name())) {
			return (vec_server[i]);
		}
	}
	for (size_t i = 0; i < vec_server.size(); i++) {
		if (vec_server[i]->check_listen(client_fd_info)) {
			return (vec_server[i]);
		}
	}
	return (vec_server[0]);
}
