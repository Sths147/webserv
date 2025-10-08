#include "ClientCgi.hpp"
#define MAX_BUFFER			1048

ClientCgi::ClientCgi(const int &in, const int &out) : _fd_in(in), _write_finish(false), _fd_out(out), _read_finish(false) {

}

ClientCgi::~ClientCgi( void ) {

}

#include <unistd.h>
#include <sys/epoll.h>

void				ClientCgi::del_epoll_and_close( int epoll_fd ) {
	if (this->_fd_in != -1) {
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, this->_fd_in, NULL);
		close(this->_fd_in);
	}
	if (this->_fd_out != -1) {
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, this->_fd_out, NULL);
		close(this->_fd_out);
	}
}

void				ClientCgi::set_pid( pid_t &pid ) {
	this->_pid = pid;
}
#include <vector>
void				ClientCgi::add_body_request(const std::vector<char> &tmp ) {
	this->_body_request = tmp.data();
}

#include <cstring>
int					ClientCgi::read_cgi_output( void ) {

	char				tmp[MAX_BUFFER + 1];
	std::memset(&tmp, 0, sizeof(tmp));

	ssize_t bytes = read(this->_fd_out, &tmp, MAX_BUFFER);
	if (bytes < 0) {
		return (-1);
	}
	if (bytes == 0) {
		return (1);
	}
	this->_output_cgi += tmp;
	return (0);
}


#include <climits>
int					ClientCgi::write_cgi_input( void ) {

	ssize_t bytes = write(this->_fd_in, this->_body_request.c_str(), std::min(this->_body_request.length(), static_cast<size_t>(SSIZE_MAX)));

	if (bytes < 0) {
		return (-1);
	}
	if (static_cast<size_t>(bytes) == this->_body_request.length()){
		return (1);
	}
	this->_body_request.erase(0, bytes);
	return (0);
}
