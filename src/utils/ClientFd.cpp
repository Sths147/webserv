
#include "ClientFd.hpp"
#include <climits>
#include <sys/socket.h>

#define RESET "\033[0m"
#define RED "\033[31m"

ClientFd::ClientFd( void ) {}

ClientFd::ClientFd(const Listen &listen ) : _header_saved(false), _server(NULL), _time_to_reset(std::time(NULL) + TIMEOUT), _host_port(listen.ip, listen.port), _response("") {

	this->_response =
		"HTTP/1.1 404 OK\r\n"
		"Content-Type: text/html\r\n"
		"Connection: close\r\n"
		"\r\n"
		"<html><head><title>Welcome</title></head>"
		"<body><h2>Hello, World!</h2></body></html>";
}



ClientFd &ClientFd::operator=( const ClientFd &other )
{
	// std::cout << BLUE << "Operator '=' is Called" << RESET << std::endl;
	if (this != &other){
		this->_buffer = other._buffer;
		this->_header_saved = other._header_saved;
		this->_header = other._header;
		this->_time_to_reset = other._time_to_reset;
		this->_host_port = other._host_port;
		this->_respons = other._respons;
		this->_response = other._response;
	}
	return	*this;
}

void			ClientFd::print_vec(std::vector<char> &vec) {

	if (vec.begin() == vec.end()) {
		return;
	}
	std::stringstream ss;
	for (std::vector<char>::iterator it = vec.begin(); it != vec.end(); it++) {
		ss << *it;
	}
	std::cout << ss.str() << std::endl;
}


void ClientFd::find_server_from_map(std::vector<Server *> &vec_server){

	if (!this->_header_saved){
		return;
	}

	for (size_t i = 0; i < vec_server.size(); i++) {

		if (vec_server[i]->check_listen(this->_host_port)) { // && this->_request.check_hosts(vec_server[i]->get_server_name())) {

			this->_server = vec_server[i];
		}
	}
	for (size_t i = 0; i < vec_server.size(); i++) {

		if (vec_server[i]->check_listen(this->_host_port)) {

			this->_server = vec_server[i];
		}
	}
	this->_server = vec_server[0];
}






static bool find_end_of_headers(std::vector<char>& buffer)
{
	if (buffer.size() < 4)
		return (0);
	else
	{
		std::vector<char>::iterator it = buffer.end() - 4;
		if (*it++ != '\r')
			return (0);
		if (*it++ != '\n')
			return (0);
		if (*it++ != '\r')
			return (0);
		if (*it != '\n')
			return (0);
	}
	return (1);
}

static bool	max_size_reached(std::vector<char>& body, Server *server)
{
	if (server->get_client_max_body_size() && (body.size() > server->get_client_max_body_size())) {
		return (1);
	}
	return (0);
}

void	ClientFd::add_buffer( char *str, std::vector<Server *> &vec_server ) {

	for (size_t i = 0; str[i]; i++) {

		this->_buffer.push_back(str[i]);
		if (!this->_header_saved && find_end_of_headers(this->_buffer)) {

			this->_header = this->_buffer;
			this->_buffer.clear();
			this->_header_saved = true;
			this->find_server_from_map(vec_server);
			// this->_request.add_header(this->_header)
		}
	}
	if (this->_header_saved && max_size_reached(this->_buffer, this->_server)){
		;
		// this->_request.set_return_code(413);
	}

	// this->print_vec(this->_header);
	// this->print_vec(this->_buffer);
}

bool		ClientFd::send_response( int client_fd ) {

	ssize_t bytes = send(client_fd, this->_response.c_str(), std::min(this->_response.length(), static_cast<size_t>(SSIZE_MAX)), 0);

	if (bytes < 0)
		throw (1);
	if (static_cast<size_t>(bytes) == this->_response.length())
		return true;

	this->_response.erase(0, bytes);
	return false;
}





Listen	ClientFd::get_listen( void ) { return (this->_host_port); }

void	ClientFd::refresh( void ) { this->_time_to_reset = std::time(NULL) + TIMEOUT; }

bool	ClientFd::check_timeout( void ) {
	if (this->_time_to_reset < std::time(NULL)){
		std::cout << RED << "Timeout" << RESET<<std::endl;
		return (false);
	}
	return (true);
}

#include <unistd.h>
#include <sys/epoll.h>
void	ClientFd::del_epoll_and_close( int epoll_fd, int client_fd ) {
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
	close(client_fd);
}





// bool	check_body(Request& request, Server& server, std::vector<char>& body)
// {
// 	if (server.get_client_max_body_size() &&  (body.size() >  server.get_client_max_body_size()))
// 	{
// 		request.set_return_code(413);
// 		return (0);
// 	}
// 	if (request.get_header("Content-Length").compare("Unexisting header"))
// 	{
// 		std::stringstream ss(request.get_header("Content-Length"));
// 		size_t	len;
// 		ss >> len;
// 		if (body.size() == 1 && body[0] == '\n')
// 			body.erase(body.begin());
// 		if (len != body.size())
// 		{
// 			request.set_return_code(400);
// 			return (0);
// 		}
// 		return (1);
// 	}
// 	return (1);
// }
