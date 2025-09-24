#define MAX_EVENTS			10
#define	MAX_REQUESTS_LINE	20
#define MAX_EVENTS			10
#define MAX_BUFFER			1024

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <map>
#include <csignal>

#include "Config.hpp"
#include "Server.hpp"
#include "MyException.hpp"
#include "Response.hpp"
#include "ClientFd.hpp"

void		set_nonblocking(int socket_fd);
bool		check_add_new_connection( const std::vector<Server *> &vec_server,	int &event_fd, int &epoll_fd, std::map<int, ClientFd> &client_socket_server);
Server		*find_server_from_map(Listen client_fd_info, std::vector<Server *> &vec_server, Request &req1);
bool		epollctl(int epoll_fd, int client_fd, const int events, int op);

bool interrupted = true;
void signalHandler(int) {
	interrupted = false;
}



int sig = false;
void sigpipehandler(int){
	sig = true;
	interrupted = false;
}

#include <cstring>

int main(int ac, char **av)
{
	//----------------------------parsing of the config file + creation of every instanse of server with his config----------------------------


	int epoll_fd;


	std::vector<Server *> vec_server;
	if (ac == 2){

		epoll_fd = epoll_create(1);
		if (epoll_fd < 0){
			std::cerr << "epoll_create failed" << std::endl;
			return (1);
		}
		try	{

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
		}
		catch(const std::exception& e)
		{
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

	std::cout << YELLOW <<"\n\tMain loop\n" << RESET << std::endl;

	int nfds;
	struct epoll_event events[MAX_EVENTS];
	std::map<int, ClientFd> client_socket_server;
	std::signal(SIGINT, signalHandler);

	// Ou capturer le signal
	signal(SIGPIPE, sigpipehandler);
	try {

		while (interrupted) {

			nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
			// std::cout << "epoll_wait" << std::endl;
			if (nfds < 0) {

				if (sig) std::cout << "error sigpiep : " << strerror(errno) << std::endl;
				else if (!interrupted) std::cout << "\nSIGINT detected." << std::endl;
				else std::cerr << "Epoll wait failed" << std::endl;
				for (std::map<int , ClientFd>::iterator it = client_socket_server.begin(); it != client_socket_server.end();){
					it->second.del_epoll_and_close(epoll_fd);
					client_socket_server.erase(it++);
				}
				for (size_t i = 0; i < vec_server.size(); i++) {delete vec_server[i];}
				close(epoll_fd);
				return (1);
			}
			else if ( nfds > 0 ){

				for (int i = 0; i < nfds; i++) {

					int client_fd = events[i].data.fd;
					// std::cout << "request fd = " << client_fd << std::endl;
					if (!check_add_new_connection(vec_server, client_fd, epoll_fd, client_socket_server)) {

						if (events[i].events & EPOLLIN) {

							std::cout << "EPOLLIN" << std::endl;
							// Handle client data
							std::vector<char>	buffer;

							ssize_t				bytes = 1;
							do {
								char				tmp;
								bytes = recv(client_fd, &tmp, sizeof(char), 0);
								buffer.push_back(tmp);
							}
							while (bytes > 0);
							if (buffer.empty())
								throw std::runtime_error("Empty request");

							Request	req1(buffer);

							Server *serv = find_server_from_map(client_socket_server[client_fd].get_listen(), vec_server, req1);
							Response res(req1, *serv);
							// res.write_response(client_fd);

							client_socket_server[client_fd].set_response(res);


							if (!epollctl(epoll_fd, client_fd, EPOLLOUT, EPOLL_CTL_MOD)){
								close(client_fd);
								break;
							}

						}
						 else if (events[i].events & EPOLLOUT ) {
							// std::cout << "EPOLLOUT" << std::endl;
							client_socket_server[client_fd].send(client_fd);
							// client_socket_server[client_fd].del_epoll_and_close(epoll_fd);
							// if (!epollctl(epoll_fd, client_fd, EPOLLIN, EPOLL_CTL_MOD)){
							// 	close(client_fd);
							// 	return (true);
							// }
							// return 1;
						} else if (events[i].events & EPOLLRDHUP ) {
								client_socket_server[client_fd].del_epoll_and_close(epoll_fd);
								client_socket_server.erase(client_fd);
						}
					}
				}

			} else {

				// if (client_socket_server.begin() == client_socket_server.end())
				// 	continue;
				std::cout << YELLOW << "\ncheck_timeout :"<< RESET;
				for (std::map<int, ClientFd>::iterator it = client_socket_server.begin(); it != client_socket_server.end();) {
					std::cout << "\n\ton clientfd "<< it->first;
					if (!it->second.check_timeout()){
						it->second.del_epoll_and_close(epoll_fd);
						client_socket_server.erase(it++);
					} else {
						++it;
					}
				}
				std::cout<<"\n"<<std::endl;
				// continue;
			}
		}
	}
	catch (std::exception& e) {
		std::cout << "BIG ERROR " << e.what() << std::endl;
	}

	for (std::map<int , ClientFd>::iterator it = client_socket_server.begin(); it != client_socket_server.end();){
		it->second.del_epoll_and_close(epoll_fd);
		client_socket_server.erase(it++);
	}
	for (size_t i = 0; i < vec_server.size(); i++) {delete vec_server[i];}
	close(epoll_fd);
	if (sig){ std::cout << "error sigpiep : " << strerror(errno) << std::endl;
		return 1;
	}else if (!interrupted){
		std::cout << "\nSIGINT detected." << std::endl;
		return (1);
	}
	return (0);
}

