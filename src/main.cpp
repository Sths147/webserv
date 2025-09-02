#define MAX_EVENTS			10
#define	MAX_REQUESTS_LINE	20
#define MAX_EVENTS			10
#define MAX_BUFFER			1024

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "Config.hpp"
#include "Server.hpp"
#include "MyException.hpp"
#include "Struct.hpp"
#include "Response.hpp"
#include <map>

void set_nonblocking(int socket_fd);
bool	check_add_new_connection( const std::vector<Server *> &vec_server,	int &event_fd, int &epoll_fd, std::map<int, Listen> &client_socket_server);
Server	*find_server_from_map(Listen client_fd_info, t_map_uint_maps_uint_vec_server &map_ip_port_vec_ptrserver, Request &req1);


int main(int ac, char **av)
{
	//----------------------------parsing of the config file + creation of every instanse of server with his config----------------------------

	int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (epoll_fd < 0){
		std::cerr << "Error : Epoll creation failed" << std::endl;
		return (1);
	}

	std::vector<Server *> vec_server;
	t_map_uint_maps_uint_vec_server map_ip_port_vec_ptr_server;
	if (ac == 2){

		try	{
			//First parsing of the config file to delete every empty and commentary line.
			Config config(av[1]);
			//Parsing of the config file.
			config.pars();
			for (size_t i = 0; i < config.nb_of_server(); i++)
			{
				// std::cout << "server n*" << i <<std::endl; //Creat every server with his config file added.
				Server *ptr = new Server(config.copy_config_server(i), epoll_fd, map_ip_port_vec_ptr_server);
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
		std::cerr << "We need a Config file to lunch the server" << std::endl;
		close(epoll_fd);
		return (1);
	}

	//----------------------------main loop event epoll add a new connection read a request response to his resquest----------------------------

	std::cout << YELLOW <<"\n\tMain loop\n" << RESET << std::endl;

	int nfds;
	struct epoll_event events[MAX_EVENTS];
	std::map<int, Listen> client_socket_server;
	try {
		while (1) {

			nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
			if (nfds < 0) {
				std::cerr << "Epoll wait failed" << std::endl;
				for (size_t i = 0; i < vec_server.size(); i++) {delete vec_server[i];}
				close(epoll_fd);
				return (1);
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

					Server *serv = find_server_from_map(client_socket_server[client_fd], map_ip_port_vec_ptr_server , req1);


					Response rep(req1, *serv);
					rep.write_response(client_fd);
					// std::cout << rep.get_connection_header() << " connection header" << std::endl;
					// std::cout << "type: " << req1.get_type() << std::endl;
					// write (client_fd, "HTTP/1.1 200 \r\n\r\n <html><body><h1>Hello buddy</h1></body></html>", 65);
					// if (rep.get_connection_header().compare("Keep-alive"))
					close(client_fd);
				}
			}
		}
	}
	catch (std::exception& e)
	{
		std::cout << "BIG ERROR " << e.what() << std::endl;
	}
	close(epoll_fd);
	return (0);
}



