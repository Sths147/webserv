
#include "ClientFd.hpp"
#include <climits>
#include <sys/socket.h>

#define RESET "\033[0m"
#define RED "\033[31m"

ClientFd::ClientFd( void ) : _request(NULL) {}

// ClientFd::ClientFd(const Listen &listen ) : _host_port(listen.ip, listen.port), _body_saved(false) ,_header_saved(false), _request(NULL), _server(NULL), _alive(true), _response("") {
// }
ClientFd::ClientFd(const Listen &listen , int fd) : _fd(fd), _host_port(listen.ip, listen.port), _body_saved(false) ,_header_saved(false), _request(NULL), _server(NULL), _alive(true), _response("") {
}

ClientFd &ClientFd::operator=( const ClientFd &other )
{
	// std::cout << BLUE << "Operator '=' is Called" << RESET << std::endl;
	if (this != &other){
		this->_host_port = other._host_port;

		this->_body_saved = other._body_saved;
		this->_buffer = other._buffer;
		this->_header_saved = other._header_saved;
		this->_header = other._header;

		// this->_request = other._request; // never copie or its will be "double free"
		this->_server = other._server;
		this->_alive = other._alive;
		this->_response = other._response;
	}
	return	*this;
}


ClientFd::~ClientFd( void ) {
	if (this->_request != NULL)
		delete this->_request;
}

Listen	ClientFd::get_listen( void ) { return (this->_host_port); }


const std::string		ClientFd::get_type() const { return(this->_request->get_type()); }
bool					ClientFd::get_body_check( void ) { return(this->_body_saved); }
bool					ClientFd::get_header_saved( void ) { return(this->_header_saved); }

void					ClientFd::print_vec(std::vector<char> &vec) {

	if (vec.begin() == vec.end()) {
		return;
	}
	std::stringstream ss;
	for (std::vector<char>::iterator it = vec.begin(); it != vec.end(); it++) {
		ss << *it;
	}
	std::cout << ss.str() << std::endl;
}

static bool find_end_of_headers(std::vector<char>& buffer)
{
	if (buffer.size() < 4){
		return (0);
	} else {

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




static bool	check_body(Request& request, Server *server, std::vector<char>& body)
{
	if (server->get_client_max_body_size() &&  (body.size() >  server->get_client_max_body_size()))
	{
		request.set_return_code(413);
		return (0);
	}
	if (request.get_header("Content-Length").compare("Unexisting header"))
	{
		std::stringstream ss(request.get_header("Content-Length"));
		size_t	len;
		ss >> len;
		if (body.size() == 1 && body[0] == '\n')
			body.erase(body.begin());
		if (len != body.size())
		{
			request.set_return_code(400);
			return (0);
		}
		return (1);
	}
	return (1);
}

void		ClientFd::add_buffer( char *str, std::vector<Server *> &vec_server ) {

	if (this->_request == NULL)
		this->_request = new Request;
	for (size_t i = 0; str[i]; i++) {

		this->_buffer.push_back(str[i]);
		if (!this->_header_saved && find_end_of_headers(this->_buffer)) {

			this->_header = this->_buffer;
			this->_buffer.clear();
			this->_header_saved = true;
			this->find_server_from_map(vec_server);
			this->_request->add_header(this->_header);
		}
	}
	if (this->_header_saved && this->_request->get_type() == "POST" && max_size_reached(this->_buffer, this->_server)){
		if(check_body(*this->_request, this->_server, this->_buffer)) {
			this->_request->add_body(this->_buffer);

		}
	}
	// this->print_vec(this->_header);
	// this->print_vec(this->_buffer);
}


void		ClientFd::find_server_from_map(std::vector<Server *> &vec_server){

	if (!this->_header_saved){
		return;
	}

	for (size_t i = 0; i < vec_server.size(); i++) {

		if (vec_server[i]->check_listen(this->_host_port) && this->_request->check_hosts(vec_server[i]->get_server_name())) {

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

bool				ClientFd::check_alive( void ) {
	return (this->_alive);
}

void				ClientFd::creat_response( void ) {

	if (this->_request == NULL) {
		return ;
	}
	Response rep(*this->_request, *this->_server);

	if (!this->_request->get_header("Connection").compare("Keep-alive") || this->_request->get_header("Connection").compare("Unexisting header")){
		std::cout << "ALIVE" << std::endl;
		this->_alive = true;
	} else {
		this->_alive = false;
	}
	delete this->_request;
	this->_request = NULL;
	this->_response = rep.construct_response();
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




















#include <unistd.h>
#include <sys/epoll.h>
void		ClientFd::del_epoll_and_close( int epoll_fd) {
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, this->_fd, NULL);
	close(this->_fd);
}



