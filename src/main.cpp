#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Server.hpp"

int main() 
{

    Server webserv;
    webserv.Serverloop();
    

    // // Main loop
    // while (1) {
    //     nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
	// 	// std::cout
    //     if (nfds < 0) {
    //         perror("Epoll wait failed");
    //         exit(1);
    //     }

    //     for (int i = 0; i < nfds; i++) {
    //         if (events[i].data.fd == server_fd) {
    //             // New connection
    //             socklen_t client_len = sizeof(client_addr);
    //             client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    //             if (client_fd < 0) {
    //                 perror("Accept failed");
    //                 continue;
    //             }

    //             // Set client socket to non-blocking
    //             set_nonblocking(client_fd);

    //             // Add client socket to epoll
    //             ev.events = EPOLLIN | EPOLLET; // Edge-triggered
    //             ev.data.fd = client_fd;
    //             if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
    //                 perror("Epoll add client socket failed");
    //                 close(client_fd);
    //                 continue;
    //             }
    //         } else {
    //             // Handle client data
    //             int client_fd = events[i].data.fd;
    //             ssize_t bytes_read = read(client_fd, buffer, MAX_BUFFER - 1);
    //             if (bytes_read <= 0) {
    //                 // Connection closed or error
    //                 epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
    //                 close(client_fd);
    //                 continue;
    //             }

    //             buffer[bytes_read] = '\0';
    //             // Basic HTTP request parsing
    //             if (strncmp(buffer, "GET ", 4) == 0) {
    //                 // Respond with a simple HTML page
    //                 const char *response =
    //                     "HTTP/1.1 404 OK\r\n"
    //                     "Content-Type: text/html\r\n"
    //                     "Connection: close\r\n"
    //                     "\r\n"
    //                     "<html><head><title>Welcome</title></head>"
    //                     "<body><h2>Hello, World!</h2></body></html>";

    //                 write(client_fd, response, strlen(response));
    //             }

    //             // Close client socket
    //             epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
    //             close(client_fd);
    //         }
    //     }
    // }

    // // Cleanup (unreachable in this example)
    // close(server_fd);
    // close(epoll_fd);
    // return 0;
}
