
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
#include "ClientCgi.hpp"

bool epollctl(int epoll_fd, int client_fd, const int events, int op) {

	struct epoll_event ev;
	std::memset(&ev, 0, sizeof(ev));

	ev.events = EPOLLRDHUP | events ;// EPOLLIN : EPOLLOUT
	ev.data.fd = client_fd;

	// op = EPOLL_CTL_ADD
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

bool check_add_new_connection(const std::vector<Server *> &vec_server, int &event_fd, int &epoll_fd, std::map<int, Client *> &fd_to_info)
{
	// std::cout << "Debug check_add_new_connection" << std::endl;
	for (size_t i = 0; i < vec_server.size(); i++)
	{
		std::vector<int> vec_socket_fd = vec_server[i]->get_socket_fd();

		for (size_t j = 0; j < vec_socket_fd.size(); j++)
		{
			if (event_fd == vec_socket_fd[j]) {

				const std::vector<Listen> &vec_listen = vec_server[i]->get_listen();

				int client_fd;

				client_fd = accept(vec_socket_fd[j], NULL, NULL);
				if (client_fd < 0) {
					std::cerr << "accept failed :" << strerror(errno) << std::endl;
					return (true);
				}

				// Set client socket to non-blocking
				set_nonblocking(client_fd);

				if (!epollctl(epoll_fd, client_fd, EPOLLIN, EPOLL_CTL_ADD)) {
					close(client_fd);
					return (true);
				}

				fd_to_info[client_fd] = new ClientFd(vec_listen[j], client_fd, epoll_fd); // save the ClientFd with the listen struct and some other param

				std::cout << GREEN << "add new connection " << RESET << client_fd << std::endl;

				return (true);
			}
		}
	}
	return (false);
}

void	clean_fd(std::map<int, Client *> &fd_to_info, const int &epoll_fd, std::vector<Server *> &vec_server) {

	for (std::map<int, Client *>::iterator it = fd_to_info.begin(); it != fd_to_info.end(); it++) {
		if (it->first != -1){
			close(it->first);
		}
	}
	for (size_t i = 0; i < vec_server.size(); i++) {vec_server[i]->close_fd();}
	close(epoll_fd);
}

void clean_for_cgi(std::map<int, Client *> &fd_to_info, std::vector<Server *> &vec_server) {

	for (std::map<int, Client *>::iterator it = fd_to_info.begin(); it != fd_to_info.end();) {
			delete it->second;
		fd_to_info.erase(it++);
	}

	for (size_t i = 0; i < vec_server.size(); i++) {delete vec_server[i];}
}

void clean_exit(std::map<int, Client *> &fd_to_info, const int &epoll_fd, std::vector<Server *> &vec_server) {

	for (std::map<int, Client *>::iterator it = fd_to_info.begin(); it != fd_to_info.end();) {

		if (dynamic_cast<ClientFd *>(it->second) != NULL) {
			dynamic_cast<ClientFd *>(it->second)->del_epoll_and_close(epoll_fd);
			delete it->second;

		} else if (dynamic_cast<ClientCgi *>(it->second) != NULL) {
			dynamic_cast<ClientCgi *>(it->second)->del_epoll_and_close(epoll_fd);
			delete it->second;
		}
		fd_to_info.erase(it++);
	}
	for (size_t i = 0; i < vec_server.size(); i++) {delete vec_server[i];}
	close(epoll_fd);
}

void check_all_timeout( const int &epoll_fd, std::map<int, Client *> &fd_to_info) {

	for (std::map<int, Client *>::iterator it = fd_to_info.begin(); it != fd_to_info.end(); ) {

		ClientFd* ptrClientFd = dynamic_cast<ClientFd *>(it->second);
		ClientCgi* ptrClientCgi = dynamic_cast<ClientCgi *>(it->second);

		if (ptrClientFd && !ptrClientFd->check_timeout()) {
			// ptrClientFd->clean_cgi(epoll_fd, fd_to_info);
			ptrClientFd->del_epoll_and_close(epoll_fd);
			delete it->second;
			fd_to_info.erase(it++);
		}
		else if (ptrClientCgi && !ptrClientCgi->check_timeout(epoll_fd, fd_to_info)) {
			ptrClientCgi->del_epoll_and_close(epoll_fd);
			delete it->second;
			fd_to_info.erase(it++);
		}
		else {
			++it;
		}
	}
}

void	delete_client(int epoll_fd, int client_fd, std::map<int, Client *> &fd_to_info, ClientCgi* ptrClient) {
		ptrClient->del_epoll_and_close(epoll_fd);
		delete fd_to_info[client_fd];
		fd_to_info.erase(client_fd);
}

void	delete_client(int epoll_fd, int client_fd, std::map<int, Client *> &fd_to_info, ClientFd* ptrClient) {
		ptrClient->del_epoll_and_close(epoll_fd);
		delete fd_to_info[client_fd];
		fd_to_info.erase(client_fd);
}

bool	epollctl_error_gestion(int epoll_fd, int client_fd, const int events, int op, std::map<int, Client *> &fd_to_info, ClientCgi* ptrClient) {

	if (!epollctl(epoll_fd, client_fd, events, op)) {
		delete_client(epoll_fd, client_fd, fd_to_info, ptrClient);
		return false;
	}
	return true;
}

bool	epollctl_error_gestion(int epoll_fd, int client_fd, const int events, int op, std::map<int, Client *> &fd_to_info, ClientFd* ptrClient) {

	if (!epollctl(epoll_fd, client_fd, events, op)) {
		delete_client(epoll_fd, client_fd, fd_to_info, ptrClient);
		return false;
	}
	return true;
}
