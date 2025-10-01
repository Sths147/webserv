

#ifndef MAIN_UTILS_HPP
# define MAIN_UTILS_HPP

void		set_nonblocking(int socket_fd);
bool		check_add_new_connection( const std::vector<Server *> &vec_server,	int &event_fd, int &epoll_fd, std::map<int, ClientFd> &client_socket_server);
Server		*find_server_from_map(Listen client_fd_info, std::vector<Server *> &vec_server, Request &req1);
void		clean_exit(std::map<int , ClientFd> &client_socket_server, int &epoll_fd, std::vector<Server *> &vec_server);
bool		epollctl(int epoll_fd, int client_fd, const int events, int op);


# endif
