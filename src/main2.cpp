#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#define PORT 8080
#define BUFFER_MAX 1024
#define MAX_EVENTS 10


class Server
{
    private:

    public:
        Server(void) ;
        ~Server() ;
};



int main()
{
	int sockfd;
	int	one = 1;
	struct sockaddr_in	address;
	socklen_t addrlen = sizeof(address);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		perror("error opening socket");
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	bind(sockfd, (struct sockaddr *)&address, addrlen);
	listen(sockfd, 1024);


	write(1, "server listening on port 8010\n", 30);
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	int epfd = epoll_create1(EPOLL_CLOEXEC);
	struct epoll_event ev, events[MAX_EVENTS];
	ev.events = EPOLLIN;
	ev.data.fd = sockfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);





	while (1)
	{
		int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		for (int i = 0; i < nfds; i++)
		{
			if (events[i].data.fd == sockfd)
			{
				struct sockaddr_in	client_address;
				socklen_t	client_len = sizeof(client_address);
				int client_fd = accept(sockfd, (struct sockaddr *)&client_address, &client_len);
				flags = fcntl(client_fd, F_GETFL, 0);
				fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
				ev.events = EPOLLIN | EPOLLRDHUP;
				ev.data.fd = client_fd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
			}
			else {
				char buffer[BUFFER_MAX];
				int client_fd = events[i].data.fd;
				ssize_t bytes = recv(client_fd, buffer, sizeof(buffer), 0);
				std::string	req;
				while (bytes > 0)
				{
					req += buffer;
					for (int i = 0; i < BUFFER_MAX; i++)
						buffer[i] = 0;
					bytes = recv(client_fd, buffer, sizeof(buffer), 0);
				}
				std::map<int, std::string> request;
				request[client_fd] = req;
				std::cout << req << std::endl;
				write (client_fd, "HTTP/1.1 200 \r\n\r\n <html><body><h1>Hello buddy</h1></body></html>", 65);
				close(client_fd);
			}
		}
	}
	close(sockfd);
	return (0);
}
