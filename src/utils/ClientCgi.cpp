#include "ClientCgi.hpp"
#include "ClientFd.hpp"
#include "utils.hpp"
#include <cstring>
#include <sys/wait.h>
#include <climits>
#include <vector>
#define MAX_BUFFER			1048

ClientCgi::~ClientCgi( void ) {}
ClientCgi::ClientCgi(const int &in, const int &out, const int &client_fd) : _pid(-1), _fd_in(in),_fd_out(out), _response(NULL), _from_clientfd(client_fd) {

}


void				ClientCgi::set_response(Response *res) {
	this->_response = res;
}

#include <unistd.h>
#include <sys/epoll.h>

int					ClientCgi::get_fd() { return ((this->_fd_in == -1) ? (this->_fd_out) : (this->_fd_in));}


void				ClientCgi::del_epoll_and_close( int epoll_fd ) {
	if (this->_fd_in != -1) {
		std::cout << YELLOW << "cgi->_fd_in deleted "<<this->_fd_in << RESET << std::endl;
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, this->_fd_in, NULL);
		close(this->_fd_in);
		this->_fd_in = -1;
	}
	if (this->_fd_out != -1) {
		std::cout << YELLOW << "cgi->_fd_out deleted "<<this->_fd_out << RESET << std::endl;
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, this->_fd_out, NULL);
		close(this->_fd_out);
		this->_fd_out = -1;
	}
}

void					ClientCgi::set_pid( pid_t &pid ) {
	this->_pid = pid;
}


void					ClientCgi::add_body_request(const std::vector<char> &tmp ) {

	this->_body_request.append(tmp.begin(), tmp.end());
}

bool					ClientCgi::check_waitpid( pid_t &_pid ) {

	int	status;
	if (this->_pid == -1)
	{
		std::cout << "Youhouuuu" << std::endl;
		return (false);
	}
	pid_t rpid = waitpid(_pid, &status, WNOHANG);
	if (rpid == 0) {
		// std::cout << "Toujours en cours" << std::endl;
		return (true);
	} else if (rpid == _pid) {

		if (WIFEXITED(status)) {

			int code = WEXITSTATUS(status);
			if (code == 0) {

				// std::cout << "Cgi finish good" << std::endl;
			} else {

				this->_response->set_status(500);
				// std::cout << "Cgi exited with code: " << code << std::endl;
			}
		} else if (WIFSIGNALED(status)) {

			this->_response->set_status(500);
			// std::cout << "Cgi finish by signal: " << WTERMSIG(status) << std::endl;

		}else if (WIFSTOPPED(status)) {

			this->_response->set_status(500);
			// std::cout << "Cgi stop by signal: " << WSTOPSIG(status) << std::endl;

		}
		return (false);

	} else {

		this->_response->set_status(500);
		std::cerr <<"waitpid: " << rpid << std::endl;
		return (false);
	}
	return (true);
}

bool					ClientCgi::read_cgi_output( void ) {

	char				tmp[MAX_BUFFER + 1];
	std::memset(&tmp, 0, sizeof(tmp));
	ssize_t bytes = read(this->_fd_out, &tmp, MAX_BUFFER);

	if (bytes < 0) {
		throw (MyException("Error : read output cgi failed."));
	}
	this->_output_cgi += tmp;
	if (bytes <= MAX_BUFFER) {
		return (!check_waitpid(this->_pid));
	}
	return (false);
}

bool					ClientCgi::write_cgi_input( void ) {

	int	status;
	pid_t rpid = waitpid(this->_pid, &status, WNOHANG);
	if (rpid != 0) {
		return (true);
	}

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




void		ClientCgi::construct_response( const int &epoll_fd, std::map<int, Client *> &fd_to_info ) {

	ClientFd* ptrClient = dynamic_cast<ClientFd *>(fd_to_info[this->_from_clientfd]);
	std::cout << "COUCOU" << _output_cgi << std::endl;
	// this->_output_cgi.empty()
	this->_response->set_body(this->_output_cgi);
	ptrClient->set_response_str(this->_response->construct_response_cgi());

	this->_response->null_cgi();
	if (!epollctl_error_gestion(epoll_fd, this->_from_clientfd, EPOLLOUT, EPOLL_CTL_MOD, fd_to_info, ptrClient)) {
		;
	}
}

bool	ClientCgi::check_timeout( const int &epoll_fd, std::map<int, Client *> &fd_to_info ) {

	if (!Client::check_timeout()) {
		delete_client(epoll_fd, this->_from_clientfd, fd_to_info, dynamic_cast<ClientFd *>(fd_to_info[this->_from_clientfd]));
		return (false);
	}

	if (!check_waitpid(this->_pid)) {
		this->construct_response(epoll_fd, fd_to_info);
		return (false);
	}

	return (true);
}
