

#ifndef MAIN_UTILS_HPP
# define MAIN_UTILS_HPP

class ClientFd;
class ClientCgi;


void	delete_client(int epoll_fd, int client_fd, std::map<int, Client *> &fd_to_info, ClientFd* ptrClient);
void	delete_client(int epoll_fd, int client_fd, std::map<int, Client *> &fd_to_info, ClientCgi* ptrClient);
bool	epollctl_error_gestion(int epoll_fd, int client_fd, const int events, int op, std::map<int, Client *> &fd_to_info, ClientCgi* ptrClient);
bool	epollctl_error_gestion(int epoll_fd, int client_fd, const int events, int op, std::map<int, Client *> &fd_to_info, ClientFd* ptrClient);

void		set_nonblocking(int socket_fd);
bool		check_add_new_connection( const std::vector<Server *> &vec_server,	int &event_fd, int &epoll_fd, std::map<int, Client *> &client_socket_server);
void		clean_exit(std::map<int, Client *> &client_socket_server, int &epoll_fd, std::vector<Server *> &vec_server);
bool		epollctl(int epoll_fd, int client_fd, const int events, int op);
void		check_all_timeout(std::map<int, Client *> &fd_to_info, int epoll_fd);

enum TypeClient {
	CLIENTFD,
	CLIENTCGI,
	UNKNOWCLIENT
};


# endif
