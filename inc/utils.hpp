

#ifndef UTILS_HPP
# define UTILS_HPP

class ClientFd;
class ClientCgi;

void		set_nonblocking(int socket_fd);
bool		check_add_new_connection( const std::vector<Server *> &vec_server,	int &event_fd, int &epoll_fd, std::map<int, Client *> &client_socket_server);
bool		epollctl(int epoll_fd, int client_fd, const int events, int op);
void		check_all_timeout( const int epoll_fd, std::map<int, Client *> &fd_to_info );

void		clean_exit(std::map<int, Client *> &fd_to_info, const int &epoll_fd, std::vector<Server *> &vec_server);
void		clean_for_cgi(std::map<int, Client *> &fd_to_info, std::vector<Server *> &vec_server);
void		clean_fd(std::map<int, Client *> &fd_to_info, const int &epoll_fd, std::vector<Server *> &vec_server);

void	delete_client(int epoll_fd, int client_fd, std::map<int, Client *> &fd_to_info, ClientFd* ptrClient);
void	delete_client(int epoll_fd, int client_fd, std::map<int, Client *> &fd_to_info, ClientCgi* ptrClient);
bool	epollctl_error_gestion(int epoll_fd, int client_fd, const int events, int op, std::map<int, Client *> &fd_to_info, ClientCgi* ptrClient);
bool	epollctl_error_gestion(int epoll_fd, int client_fd, const int events, int op, std::map<int, Client *> &fd_to_info, ClientFd* ptrClient);

enum TypeClient {
	CLIENTFD,
	CLIENTCGI,
	UNKNOWCLIENT
};


# endif
