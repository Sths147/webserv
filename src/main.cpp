#define MAX_EVENTS			10
#define MAX_BUFFER			100

#include <map>
#include <csignal>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Config.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "utils.hpp"
#include "MyException.hpp"
#include "ClientFd.hpp"
#include "ClientCgi.hpp"

//SIGNAL SIGINT
bool interrupted = true;
void signalHandler(int) {
	interrupted = false;
}

int main(int ac, char **av)
{
	//----------------------------parsing of the config file + creation of every instanse of server with his config----------------------------
	int epoll_fd;


	std::vector<Server *> vec_server;
	if (ac == 2) {

		epoll_fd = epoll_create(1);
		if (epoll_fd < 0) {
			std::cerr << "epoll_create failed" << std::endl;
			return (1);
		}
		try {

			//First parsing of the config file to delete every empty and commentary line.
			Config config(av[1]);

			//Parsing of the config file.
			config.pars();

			//Pars all listen if an ip : 0 and port : "n" exists we accept this one first and dosent accept other ip on the same "n" port.
			config.check_lunch();
			for (size_t i = 0; i < config.nb_of_server(); i++)
			{
				// std::cout << "server n*" << i <<std::endl; //Creat every server with his config file added.
				Server *ptr = new Server(config.copy_config_server(i), epoll_fd);
				vec_server.push_back(ptr);
			}

		} catch(const std::exception& e) {

			std::cerr << e.what() << std::endl;
			for (size_t i = 0; i < vec_server.size(); i++) { delete vec_server[i]; }
			close(epoll_fd);
			return (1);
		}

	} else {
		std::cerr << "One Config file to lunch the server" << std::endl;
		return (1);
	}

	//----------------------------main loop event epoll add a new connection read a request response to his resquest----------------------------

	// std::cout << YELLOW <<"\n\tMain loop\n" << RESET << std::endl;

	int nfds;
	struct epoll_event events[MAX_EVENTS];

	std::map<int, Client *> fd_to_info;

	std::signal(SIGINT, signalHandler);
	try {

		while (interrupted) {

			nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
			if (nfds < 0) {

				if (!interrupted) std::cout << RED"\nSIGINT detected."RESET << std::endl;
				else std::cerr << "Epoll wait failed" << std::endl;
				clean_exit(fd_to_info, epoll_fd, vec_server);
				return (1);

			} else if ( nfds > 0 ) {

				for (int i = 0; i < nfds; i++) {


					int client_fd = events[i].data.fd;

					TypeClient typeclient = UNKNOWCLIENT;
					if (fd_to_info.find(client_fd) != fd_to_info.end()) {
						if (dynamic_cast<ClientFd *>(fd_to_info[client_fd]) != NULL) {
							typeclient = CLIENTFD;
						} else if (dynamic_cast<ClientCgi *>(fd_to_info[client_fd]) != NULL) {
							typeclient = CLIENTCGI;
						}
					}

					if (events[i].events & EPOLLIN) {

						if (typeclient != UNKNOWCLIENT || !check_add_new_connection(vec_server, client_fd, epoll_fd, fd_to_info)) {

							fd_to_info[client_fd]->refresh();

							if (typeclient == CLIENTFD) {

								ClientFd* ptrClient = dynamic_cast<ClientFd *>(fd_to_info[client_fd]);

								char				tmp[MAX_BUFFER];

								ssize_t bytes = recv(client_fd, &tmp, MAX_BUFFER , MSG_DONTWAIT);
								if (bytes < 0) {
									delete_client(epoll_fd, client_fd, fd_to_info, ptrClient);
									continue;
								}
								else if (bytes == 0) {
									delete_client(epoll_fd, client_fd, fd_to_info, ptrClient);
									continue;
								}
								ptrClient->add_buffer(tmp, vec_server, bytes);
								if (ptrClient->get_header_saved() && !(ptrClient->get_type() == "POST")) {

									if (ptrClient->creat_response(fd_to_info, vec_server)){
										continue;
									}
									if (!epollctl_error_gestion(epoll_fd, client_fd, EPOLLOUT, EPOLL_CTL_MOD, fd_to_info, ptrClient)) {
										continue;
									}
								}
								else if (ptrClient->get_header_saved() && (ptrClient->get_type() == "POST") && ptrClient->get_body_check()) {

									if (ptrClient->creat_response(fd_to_info, vec_server)) {
										continue;
									}
									if (!epollctl_error_gestion(epoll_fd, client_fd, EPOLLOUT, EPOLL_CTL_MOD, fd_to_info, ptrClient)) {
										continue;
									}
								}
							} else if (typeclient == CLIENTCGI) {

								ClientCgi* ptrClient = dynamic_cast<ClientCgi *>(fd_to_info[client_fd]);

								try
								{
									int rv = ptrClient->read_cgi_output();
									if (rv == true) {
										ptrClient->construct_response(epoll_fd, fd_to_info);
										delete_client(epoll_fd, client_fd, fd_to_info, ptrClient);
										continue;
									}
								}
								catch(const std::exception& e)
								{
									std::cerr << e.what() << std::endl;
									delete_client(epoll_fd, client_fd, fd_to_info, ptrClient);
									continue;
								}
							}
						}

					} else if (events[i].events & EPOLLOUT ) {

						fd_to_info[client_fd]->refresh();
						if (typeclient == CLIENTFD) {

							ClientFd* ptrClient = dynamic_cast<ClientFd *>(fd_to_info[client_fd]);

							try
							{
								if (ptrClient->send_response(client_fd) == true) {

									if (ptrClient->check_alive()) {
										if (!epollctl_error_gestion(epoll_fd, client_fd, EPOLLIN, EPOLL_CTL_MOD, fd_to_info, ptrClient)) {
											continue;
										}
										ptrClient->clean_new_request();
									} else {
										delete_client(epoll_fd, client_fd, fd_to_info, ptrClient);
									}
								}
							}
							catch(const int &e)
							{
								delete_client(epoll_fd, client_fd, fd_to_info, ptrClient);
							}
						} else if (typeclient == CLIENTCGI) {

							ClientCgi* ptrClient = dynamic_cast<ClientCgi *>(fd_to_info[client_fd]);

							try
							{
								std::cout << "here" << std::endl;
								if (ptrClient->write_cgi_input()) {
									delete_client(epoll_fd, client_fd, fd_to_info, ptrClient);
								}
							}
							catch(const std::exception& e)
							{
								std::cerr << e.what() << std::endl;
								delete_client(epoll_fd, client_fd, fd_to_info, ptrClient);
								continue;
							}
						}

					} else if ( events[i].events & EPOLLRDHUP ) {

						if (typeclient == CLIENTFD) {
							ClientFd* ptrClient = dynamic_cast<ClientFd *>(fd_to_info[client_fd]);
							delete_client(epoll_fd, client_fd, fd_to_info, ptrClient);
						} else if (typeclient == CLIENTCGI) {
							ClientCgi* ptrClient = dynamic_cast<ClientCgi *>(fd_to_info[client_fd]);
							delete_client(epoll_fd, client_fd, fd_to_info, ptrClient);
						}
					}
				}

				check_all_timeout( epoll_fd, fd_to_info);

			} else {
				check_all_timeout( epoll_fd, fd_to_info);
			}
		}

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	} catch (const int &e) {
		if (e == -42)
			return (-42);
	}
	clean_exit(fd_to_info, epoll_fd, vec_server);
	if (!interrupted) {
		std::cout << RED << "\nSIGINT detected." << RESET << std::endl;
		return (1);
	}
	return (0);
}

