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
#include "ClientFd.hpp"
#include "main_utils.hpp"
#include "MyException.hpp"



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
	if (ac == 2){

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

	std::map<int, ClientFd> fd_to_info;

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
					if (!check_add_new_connection(vec_server, client_fd, epoll_fd, fd_to_info)) {

						if (events[i].events & EPOLLIN) {

							fd_to_info[client_fd].refresh();

							char				tmp[MAX_BUFFER + 1];
							std::memset(&tmp, 0, sizeof(tmp));

							ssize_t bytes = recv(client_fd, &tmp, MAX_BUFFER , 0);
							if (bytes < 0){
								if (!epollctl(epoll_fd, client_fd, EPOLLOUT, EPOLL_CTL_MOD)) {
									fd_to_info[client_fd].del_epoll_and_close(epoll_fd, client_fd);
									fd_to_info.erase(client_fd);
									close(client_fd);
									continue;
								}
							}

							fd_to_info[client_fd].add_buffer(tmp, vec_server);
							std::cout << YELLOW << "creat_response" << RESET << std::endl;
							if (fd_to_info[client_fd].get_header_saved() && !(fd_to_info[client_fd].get_type() == "POST")) {

								if (!epollctl(epoll_fd, client_fd, EPOLLOUT, EPOLL_CTL_MOD)) {
									fd_to_info[client_fd].del_epoll_and_close(epoll_fd, client_fd);
									fd_to_info.erase(client_fd);
									close(client_fd);
									continue;
								}
								fd_to_info[client_fd].creat_response();

							}
							else if (fd_to_info[client_fd].get_header_saved() && (fd_to_info[client_fd].get_type() == "POST") && fd_to_info[client_fd].get_body_check()) {

								if (!epollctl(epoll_fd, client_fd, EPOLLOUT, EPOLL_CTL_MOD)) {
									fd_to_info[client_fd].del_epoll_and_close(epoll_fd, client_fd);
									fd_to_info.erase(client_fd);
									close(client_fd);
									continue;
								}
								fd_to_info[client_fd].creat_response();

							}

						} else if (events[i].events & EPOLLOUT ) {

							try
							{
								fd_to_info[client_fd].refresh();

								if (fd_to_info[client_fd].send_response(client_fd) == true){
									if (!fd_to_info[client_fd].check_alive()) {
										if (!epollctl(epoll_fd, client_fd, EPOLLIN, EPOLL_CTL_MOD)) {
											fd_to_info[client_fd].del_epoll_and_close(epoll_fd, client_fd);
											fd_to_info.erase(client_fd);
											close(client_fd);
											continue;
										}
									} else {
										fd_to_info[client_fd].del_epoll_and_close(epoll_fd, client_fd);
										fd_to_info.erase(client_fd);
									}
								}
							}
							catch(const int &e)
							{
								fd_to_info[client_fd].del_epoll_and_close(epoll_fd, client_fd);
								fd_to_info.erase(client_fd);
							}
						} else if ( events[i].events & EPOLLRDHUP ) {

								fd_to_info[client_fd].del_epoll_and_close(epoll_fd, client_fd);
								fd_to_info.erase(client_fd);
						}
					}
				}
			} else {

				for (std::map<int, ClientFd>::iterator it = fd_to_info.begin(); it != fd_to_info.end(); ){
					if (!it->second.check_timeout()) {
						fd_to_info[it->first].del_epoll_and_close(epoll_fd, it->first);
						fd_to_info.erase(it++);
					} else {
						it++;
					}
				}
			}
		}

	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	clean_exit(fd_to_info, epoll_fd, vec_server);
	if (!interrupted){
		std::cout << "\nSIGINT detected." << std::endl;
		return (1);
	}
	return (0);
}


							// std::vector<char>	buffer;
							// ssize_t bytes = 1;
							// do {
							// 	char				tmp;
							// 	bytes = recv(client_fd, &tmp, sizeof(char), 0);
							// 	buffer.push_back(tmp);
							// }
							// while (bytes > 0 && !find_end_of_headers(buffer));
							// if (buffer.empty())
							// 	continue ;
 							// continue;

							// Request	req1(buffer);

							// Server *serv = find_server_from_map(fd_to_info[client_fd].get_listen(), vec_server,req1);

							// std::vector<char>	body;
							// do {
							// 	char				tmp;
							// 	bytes = recv(client_fd, &tmp, sizeof(char), 0);
							// 	if (bytes > 0)
							// 		body.push_back(tmp);
							// } while (bytes > 0 && !max_size_reached(body, *serv));

							// if (check_body(req1, *serv, body))
								// req1.add_body(body);

							// Response rep(req1, *serv);
							// rep.write_response(client_fd);

							// if (!rep.get_connection_header().compare("Keep-alive")) {

							// 	fd_to_info[client_fd].refresh();

							// } else {

							// 	fd_to_info[client_fd].del_epoll_and_close(epoll_fd);
							// 	fd_to_info.erase(client_fd);

							// }
