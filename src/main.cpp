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
static bool find_end_of_headers(std::vector<char>& buffer);
static bool	max_size_reached(std::vector<char>& body, Server& server);
static bool	check_body(Request& request, Server& server, std::vector<char>& body);
bool	check_add_new_connection( const std::vector<Server *> &vec_server,	int &event_fd, int &epoll_fd, std::map<int, ClientFd> &client_socket_server);
Server	*find_server_from_map(Listen client_fd_info, std::vector<Server *> &vec_server, Request &req1);
bool interrupted = true;

void signalHandler(int) {
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

	// std::cout << YELLOW <<"\n\tMain loop\n" << RESET << std::endl;

	int nfds;
	struct epoll_event events[MAX_EVENTS];
	std::map<int, ClientFd> client_socket_server;
	std::signal(SIGINT, signalHandler);
	try {

		while (interrupted) {

			nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
			// std::cout << "epoll_wait" << std::endl;
			if (nfds < 0) {

				if (!interrupted) std::cout << "\nSIGINT detected." << std::endl;
				else std::cerr << "Epoll wait failed" << std::endl;
				for (std::map<int , ClientFd>::iterator it = client_socket_server.begin(); it != client_socket_server.end();){
					it->second.del_epoll_and_close(epoll_fd);
					client_socket_server.erase(it++);
				}
				for (size_t i = 0; i < vec_server.size(); i++) {delete vec_server[i];}
				close(epoll_fd);
				return (1);
			} else if ( nfds > 0 ) {

				for (int i = 0; i < nfds; i++) {

					int client_fd = events[i].data.fd;
					// std::cout << "request fd = " << client_fd << std::endl;
					if (!check_add_new_connection(vec_server, client_fd, epoll_fd, client_socket_server)) {

						if (events[i].events & EPOLLIN) {

					// // Handle client data
					// std::vector<char>	buffer;
					// ssize_t bytes = 1;
					// do {
					// 	char				tmp;
					// 	bytes = recv(client_fd, &tmp, sizeof(char), 0);
					// 	if (bytes > 0)
					// 		buffer.push_back(tmp);
					// }
					// while (bytes > 0 && !find_end_of_headers(buffer));
					// if (buffer.empty())
					// 	continue ;
							// Handle client data
							std::vector<char>	buffer;
							ssize_t bytes = 1;
							do {
								char				tmp;
								bytes = recv(client_fd, &tmp, sizeof(char), 0);
								buffer.push_back(tmp);
							}
							while (bytes > 0 && !find_end_of_headers(buffer));
							if (buffer.empty())
								continue ;

							Request	req1(buffer);

							Server *serv = find_server_from_map(client_socket_server[client_fd].get_listen(), vec_server,req1);
					std::vector<char>	body;
					do {
						char				tmp;
						bytes = recv(client_fd, &tmp, sizeof(char), 0);
						if (bytes > 0)
							body.push_back(tmp);
					}
					while (bytes > 0 && !max_size_reached(body, *serv));
					// std::cout << "--------NOW DISPLAYING BODY-----" << std::endl;
					// for (std::vector<char>::iterator it = body.begin(); it != body.end(); it++)
					// 	std::cout << *it << std::ends;
					// std::cout << "--------FINISHED DISPLAYING BODY-----" << std::endl;
					if (check_body(req1, *serv, body))
						req1.add_body(body);
					// std::cout << "Body size|" << body.size() << std::endl;
					// std::cout << "Body2 size|" << req1.get_body().size() << std::endl;

					// std::cout << req1.get_return_code() << std::endl;
					Response rep(req1, *serv);
					rep.write_response(client_fd);
					// std::cout << rep.get_connection_header() << " connection header" << std::endl;
					// std::cout << "type: " << req1.get_type() << std::endl;
					// if (rep.get_connection_header().compare("Keep-alive"))

					client_socket_server[client_fd].refresh();
					// client_socket_server[client_fd].del_epoll_and_close(epoll_fd);
					// client_socket_server.erase(client_fd);
							// std::vector<char>	body;
							// do {
							// 	char				tmp;
							// 	bytes = recv(client_fd, &tmp, sizeof(char), 0);
							// 	body.push_back(tmp);
							// }
							// while (bytes > 0 && !max_size_reached(body, *serv));
							// if (serv->get_client_max_body_size() && (body.size() > serv->get_client_max_body_size()))
							// 	req1.set_return_code(413);
							// else
							// 	req1.add_body(body);

							// Response rep(req1, *serv);
							// rep.write_response(client_fd);
							if (!rep.get_connection_header().compare("Keep-alive")) {
								client_socket_server[client_fd].refresh();
							} else{
								client_socket_server[client_fd].del_epoll_and_close(epoll_fd);
								client_socket_server.erase(client_fd);
							}
						} else if (events[i].events & EPOLLRDHUP ) {
								client_socket_server[client_fd].del_epoll_and_close(epoll_fd);
								client_socket_server.erase(client_fd);
						}
					}
				}
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
	if (!interrupted){
		std::cout << "\nSIGINT detected." << std::endl;
		return (1);
	}
	return (0);

}

static bool find_end_of_headers(std::vector<char>& buffer)
{
	if (buffer.size() < 4)
		return (0);
	else
	{
		std::vector<char>::iterator it = buffer.end() - 4;
		if (*it++ != '\r')
			return (0);
		if (*it++ != '\n')
			return (0);
		if (*it++ != '\r')
			return (0);
		if (*it++ != '\n')
			return (0);
	}
	// std::cout << "FINI" << std::endl;
	return (1);
}

static bool	max_size_reached(std::vector<char>& body, Server& server)
{
	if (server.get_client_max_body_size() &&  (body.size() >  server.get_client_max_body_size()))
	{
		return (1);
	}
	return (0);
}

static bool	check_body(Request& request, Server& server, std::vector<char>& body)
{
	if (server.get_client_max_body_size() &&  (body.size() >  server.get_client_max_body_size()))
	{
		request.set_return_code(413);
		return (0);
	}
	if (request.get_header("Content-Length").compare("Unexisting header"))
	{
		std::stringstream ss(request.get_header("Content-Length"));
		size_t	len;
		ss >> len;
		if (body.size() == 1 && body[0] == '\n')
			body.erase(body.begin());
		if (len != body.size())
		{
			request.set_return_code(400);
			return (0);
		}
		return (1);
	}
	return (1);
}
