
#include "ClientFd.hpp"
#include <climits>
#include <sys/socket.h>

#define RESET "\033[0m"
#define RED "\033[31m"

ClientFd::ClientFd( void ) : _request(NULL) {}

ClientFd::ClientFd(const Listen &listen , int fd, int epoll_fd) : _fd(fd), _host_port(listen.ip, listen.port), _body_saved(false) ,_header_saved(false), _request(NULL), _res(NULL), _server(NULL), _alive(true), _response(""), _epoll_fd(epoll_fd) {
}

ClientFd &ClientFd::operator=( const ClientFd &other )
{
	// std::cout << BLUE << "Operator '=' is Called" << RESET << std::endl;
	if (this != &other) {
		this->_host_port = other._host_port;

		this->_body_saved = other._body_saved;
		this->_buffer = other._buffer;
		this->_header_saved = other._header_saved;
		this->_header = other._header;
		this->_server = other._server;
		this->_alive = other._alive;
		this->_response = other._response;
	}
	return	*this;
}

void	ClientFd::clean_new_request( void ){

	if (this->_request != NULL){
		delete this->_request;
		this->_request = NULL;
	}
	if (this->_res != NULL){
		delete this->_res;
		this->_res = NULL;
	}
	this->_header_saved = false;
	this->_body_saved = false;
}

ClientFd::~ClientFd( void ) {
	this->clean_new_request();
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
	if (buffer.size() < 4) {
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

static bool	body_size_reached(size_t buffer_size, std::string content_length)
{
	std::stringstream ss;
	size_t				len;
	ss << content_length;
	ss >> len;
	if (buffer_size >= len)
		return (true);
	return (false);
}

void		ClientFd::add_buffer( char *str, std::vector<Server *> &vec_server, size_t bytes_read ) {

	if (this->_request == NULL)
		this->_request = new Request;
	for (size_t i = 0; i < bytes_read ; i++) {

		this->_buffer.push_back(str[i]);
		if (!this->_header_saved && find_end_of_headers(this->_buffer)) {

			this->_header = this->_buffer;
			this->_buffer.clear();
			this->_header_saved = true;
			this->find_server_from_map(vec_server);
			this->_request->add_header(this->_header);
		}
	}

	if (this->_header_saved && this->_request->get_type() == "POST" && (max_size_reached(this->_buffer, this->_server) || body_size_reached(this->_buffer.size(), this->_request->get_header("Content-Length")))) {
		if(check_body(*this->_request, this->_server, this->_buffer)) {
			this->_body_saved = true;
			this->_request->add_body(this->_buffer);

		}
	}
}


void		ClientFd::find_server_from_map(std::vector<Server *> &vec_server) {

	if (!this->_header_saved) {
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

int				ClientFd::creat_response( std::map<int, Client *> &fd_to_info, std::vector<Server *> &vec_server) {

	if (this->_request == NULL) {
		return 0;
	}
	this->_res = new Response(*this->_request, *this->_server, fd_to_info, this->_epoll_fd, this->_fd, vec_server);

	if (!this->_request->get_header("Connection").compare("Keep-alive") || this->_request->get_header("Connection").compare("Unexisting header")) {
		this->_alive = true;
	} else {
		this->_alive = false;
	}
	delete this->_request;
	this->_request = NULL;

	if (this->_res->get_cgi_status()) {
		return (1);
	}

	this->_response = this->_res->construct_response();
	return (0);
}

void		ClientFd::set_response( const std::string &str){
	this->_response = str;
}

bool		ClientFd::send_response( int client_fd ) {


	// std::cout	<< "----------------------------------------send_response: \n'"
	// 			<< this->_response << "'\n"<< std::endl;


	ssize_t bytes = send(client_fd, this->_response.c_str(), std::min(this->_response.length(), static_cast<size_t>(SSIZE_MAX)), MSG_DONTWAIT);

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
	if (this->_fd != -1) {
		std::cout << YELLOW << "fd deleted "<<this->_fd << RESET << std::endl;
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, this->_fd, NULL);
		close(this->_fd);
		this->_fd = -1;
	}
}



