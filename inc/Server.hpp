
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 8080 // to change
#define MAX_EVENTS 10 // to change
#define MAX_BUFFER 1024 // to change

class Server
{

    private:
        int     _socket_fd, _epoll_fd ;
        struct sockaddr_in _server_addr, _client_addr;
        struct epoll_event _ev, _events[MAX_EVENTS];

        void initSocket(void);
        void bindlistensocket(void);
        void setnonblocking(int sockfd);


    public:
        Server(void);
        void Server::Serverloop(void);
        ~Server();
};