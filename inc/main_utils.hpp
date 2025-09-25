

#ifndef MAIN_UTILS_HPP
# define MAIN_UTILS_HPP

void		set_nonblocking(int socket_fd);
bool		find_end_of_headers(std::vector<char>& buffer);
bool		max_size_reached(std::vector<char>& body, Server& server);
bool		check_body(Request& request, Server& server, std::vector<char>& body);
bool		check_add_new_connection( const std::vector<Server *> &vec_server,	int &event_fd, int &epoll_fd, std::map<int, ClientFd> &client_socket_server);
Server		*find_server_from_map(Listen client_fd_info, std::vector<Server *> &vec_server, Request &req1);
void		clean_exit(std::map<int , ClientFd> &client_socket_server, int &epoll_fd, std::vector<Server *> &vec_server);

# endif
