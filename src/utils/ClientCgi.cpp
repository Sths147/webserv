#include "ClientCgi.hpp"
#define MAX_BUFFER			1048

ClientCgi::ClientCgi(const int &in, const int &out, const int &client_fd) : _fd_in(in), _write_finish(false), _fd_out(out), _read_finish(false), _response(NULL), _from_clientfd(client_fd) {

}

ClientCgi::~ClientCgi( void ) {

}

void				ClientCgi::set_response(Response *res) {
	this->_response = res;
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
#include <sys/wait.h>
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
	if (bytes < MAX_BUFFER) {
		return (1);
	}

	int	status;
	pid_t rpid = waitpid(this->_pid, &status, WNOHANG);
	if (rpid != 0) {
		return (1);
	}
	return (0);
}


#include <climits>
bool					ClientCgi::write_cgi_input( void ) {

	ssize_t bytes = write(this->_fd_in, this->_body_request.c_str(), std::min(this->_body_request.length(), static_cast<size_t>(SSIZE_MAX)));

	if (bytes < 0) {
		throw (MyException("Error : write in cgi failed."));
	}
	if (static_cast<size_t>(bytes) == this->_body_request.length()){
		return (true);
	}
	this->_body_request.erase(0, bytes);

	return (false);
}

#include "ClientFd.hpp"
#include "main_utils.hpp"
void		ClientCgi::construct_response( const int &epoll_fd, std::map<int, Client *> &fd_to_info ) {

	ClientFd* ptrClient = dynamic_cast<ClientFd *>(fd_to_info[this->_from_clientfd]);
	this->_response->set_body(this->_output_cgi);
	ptrClient->set_response(this->_response->construct_response_cgi());

	if (!epollctl_error_gestion(epoll_fd, this->_from_clientfd, EPOLLOUT, EPOLL_CTL_MOD, fd_to_info, ptrClient)) {
		;
	}
}

// bool	ClientCgi::check_timeout( void ){

// 	int	status;
// 	pid_t rpid = waitpid(this->_pid, &status, WNOHANG);
// 	if (rpid != 0) {
// 		std::cout << "cgi_finish" << std::endl;
// 		return (true);
// 	}
// 	return (Client::check_timeout());
// }
