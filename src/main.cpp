#define MAX_EVENTS			10
#define	MAX_REQUESTS_LINE	20
#define MAX_EVENTS			10
#define MAX_BUFFER			1048

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
#include "main_utils.hpp"
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

				if (!interrupted) std::cout << "\nSIGINT detected." << std::endl;
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

								char				tmp[MAX_BUFFER + 1];
								std::memset(&tmp, 0, sizeof(tmp));

								ssize_t bytes = recv(client_fd, &tmp, MAX_BUFFER , 0);
								if (bytes < 0) {
									if (!epollctl(epoll_fd, client_fd, EPOLLOUT, EPOLL_CTL_MOD)) {
										ptrClient->del_epoll_and_close(epoll_fd);
										delete fd_to_info[client_fd];
										fd_to_info.erase(client_fd);
										close(client_fd);
										continue;
									}
								}

								ptrClient->add_buffer(tmp, vec_server);
								if (ptrClient->get_header_saved() && !(ptrClient->get_type() == "POST")) {


									if (ptrClient->creat_response(fd_to_info)){
										continue;
									}

									if (!epollctl(epoll_fd, client_fd, EPOLLOUT, EPOLL_CTL_MOD)) {
										ptrClient->del_epoll_and_close(epoll_fd);
										delete fd_to_info[client_fd];
										fd_to_info.erase(client_fd);
										close(client_fd);
										continue;
									}
								}
								else if (ptrClient->get_header_saved() && (ptrClient->get_type() == "POST") && ptrClient->get_body_check()) {

									// std::cout << YELLOW << "creat_response POST" << RESET << std::endl;

									if (ptrClient->creat_response(fd_to_info)) {
										continue;
									}
									if (!epollctl(epoll_fd, client_fd, EPOLLOUT, EPOLL_CTL_MOD)) {
										ptrClient->del_epoll_and_close(epoll_fd);
										delete fd_to_info[client_fd];
										fd_to_info.erase(client_fd);
										close(client_fd);
										continue;
									}
								}
							} else if (typeclient == CLIENTCGI) {

								ClientCgi* ptrClient = dynamic_cast<ClientCgi *>(fd_to_info[client_fd]);

								int rv = ptrClient->read_cgi_output();
								if (rv == 0) {
									continue;
								}
								if (rv == 1 || rv < 0) {
									ptrClient->construct_response(epoll_fd, fd_to_info);
									// std::cerr << "read cgi finish." << std::endl;
									ptrClient->del_epoll_and_close(epoll_fd);
									delete fd_to_info[client_fd];
									fd_to_info.erase(client_fd);
									close(client_fd);
								}

								continue;
							}
						}

					} else if (events[i].events & EPOLLOUT ) {

						fd_to_info[client_fd]->refresh();

						if (typeclient == CLIENTFD) {

							ClientFd* ptrClient = dynamic_cast<ClientFd *>(fd_to_info[client_fd]);

							try
							{
								if (ptrClient->send_response(client_fd) == true) {

									if (!ptrClient->check_alive()) {
										if (!epollctl(epoll_fd, client_fd, EPOLLIN, EPOLL_CTL_MOD)) {
											ptrClient->del_epoll_and_close(epoll_fd);
											delete fd_to_info[client_fd];
											fd_to_info.erase(client_fd);
											close(client_fd);
											continue;
										}
									} else {
										ptrClient->del_epoll_and_close(epoll_fd);
										delete fd_to_info[client_fd];
										fd_to_info.erase(client_fd);
									}

								}
							}
							catch(const int &e)
							{
								ptrClient->del_epoll_and_close(epoll_fd);
								delete fd_to_info[client_fd];
								fd_to_info.erase(client_fd);
							}
						} else if (typeclient == CLIENTCGI) {

							ClientCgi* ptrClient = dynamic_cast<ClientCgi *>(fd_to_info[client_fd]);

							int rv = ptrClient->write_cgi_input();
							if (rv == 1 || rv < 0) {
								ptrClient->del_epoll_and_close(epoll_fd);
								delete fd_to_info[client_fd];
								fd_to_info.erase(client_fd);
								close(client_fd);
							}
						}

					} else if ( events[i].events & EPOLLRDHUP ) {

						if (typeclient == CLIENTFD) {
							ClientFd* ptrClient = dynamic_cast<ClientFd *>(fd_to_info[client_fd]);

							ptrClient->del_epoll_and_close(epoll_fd);
							delete fd_to_info[client_fd];
							fd_to_info.erase(client_fd);
						} else if (typeclient == CLIENTCGI) {
							// to something
						}
					}
				}
				check_all_timeout(fd_to_info, epoll_fd);
			} else {

				check_all_timeout(fd_to_info, epoll_fd);
			}
		}

	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	clean_exit(fd_to_info, epoll_fd, vec_server);
	if (!interrupted) {
		std::cout << "\nSIGINT detected." << std::endl;
		return (1);
	}
	return (0);
}

