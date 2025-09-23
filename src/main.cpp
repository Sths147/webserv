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

void set_nonblocking(int socket_fd);
bool	check_add_new_connection( const std::vector<Server *> &vec_server,	int &event_fd, int &epoll_fd, std::map<int, ClientFd> &client_socket_server);
Server	*find_server_from_map(Listen client_fd_info, std::vector<Server *> &vec_server, Request &req1);
bool interrupted = false;

void signalHandler(int) {
	interrupted = true;
}

int main(int ac, char **av)
{
	//----------------------------parsing of the config file + creation of every instanse of server with his config----------------------------


	int epoll_fd;


	std::vector<Server *> vec_server;
	if (ac == 2){

		epoll_fd = epoll_create1(EPOLL_CLOEXEC);
		if (epoll_fd < 0){
			std::cerr << "Error : Epoll creation failed" << std::endl;
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
	try {
		while (1) {

			nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
			if (nfds < 0) {
				if (interrupted) std::cerr << "SIGINT detected." << std::endl;
				else std::cerr << "Epoll wait failed" << std::endl;
				for (std::map<int , ClientFd>::iterator it = client_socket_server.begin(); it != client_socket_server.end();){
					it->second.del_epoll_and_close(epoll_fd);
					client_socket_server.erase(it++);
				}
				for (size_t i = 0; i < vec_server.size(); i++) {delete vec_server[i];}
				close(epoll_fd);
				return (1);
			}
			else if (nfds == 0){
				// std::cout << YELLOW << "\ncheck_timeout :"<< RESET;
				for (std::map<int, ClientFd>::iterator it = client_socket_server.begin(); it != client_socket_server.end();) {
					// std::cout << "\n\ton clientfd "<< it->first;
					if (!it->second.check_timeout()){
						it->second.del_epoll_and_close(epoll_fd);
						client_socket_server.erase(it++);
					} else {
						++it;
					}
				}
				// std::cout<<"\n"<<std::endl;
				continue;
			}

			for (int i = 0; i < nfds; i++) {

				int client_fd = events[i].data.fd;
				// std::cout << "request fd = " << client_fd << std::endl;
				if (!check_add_new_connection(vec_server, client_fd, epoll_fd, client_socket_server)) {


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
						throw std::runtime_error("empty request");

					// std::map<int, Request*> request;
					Request	req1(buffer);
					// request[client_fd] = &req1;

					// std::cout << "Request From : " << client_socket_server[client_fd].get_listen().ip << ":" << client_socket_server[client_fd].get_listen().port << std::endl;
					Server *serv = find_server_from_map(client_socket_server[client_fd].get_listen(), vec_server,req1);

					Response rep(req1, *serv);
					rep.write_response(client_fd);
					// std::cout << rep.get_connection_header() << " connection header" << std::endl;
					// std::cout << "type: " << req1.get_type() << std::endl;
					// if (rep.get_connection_header().compare("Keep-alive"))

					client_socket_server[client_fd].del_epoll_and_close(epoll_fd);
					client_socket_server.erase(client_fd);
				}
			}
		}
	}
	catch (std::exception& e)
	{
		std::cout << "BIG ERROR " << e.what() << std::endl;
	}
	for (std::map<int , ClientFd>::iterator it = client_socket_server.begin(); it != client_socket_server.end(); ++it){
		it->second.del_epoll_and_close(epoll_fd);
		client_socket_server.erase(it);
	}
	for (size_t i = 0; i < vec_server.size(); i++) {delete vec_server[i];}
	close(epoll_fd);
	return (0);
}

