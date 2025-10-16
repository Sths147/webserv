/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sithomas <sithomas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 14:43:37 by sithomas          #+#    #+#             */
/*   Updated: 2025/10/16 11:25:30 by sithomas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sstream>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctime>
#include "utils.hpp"

static std::string		reason_phrase(unsigned short int& code);
static std::string		reconstruct_path(std::string s1, std::string s2);

// Response::Response(): _status_code(0) {}
Response::~Response()
{
	if (this->_cgi_get != NULL) {
		ClientCgi *ptr_cgi1 = dynamic_cast<ClientCgi *>(this->_cgi_get);
		ptr_cgi1->del_epoll_and_close(this->_epoll_fd);
		delete this->_cgi_get;
		this->_fd_to_info.erase(ptr_cgi1->get_fd());
	}

	if (this->_cgi_post != NULL) {
		ClientCgi *ptr_cgi2 = dynamic_cast<ClientCgi *>(this->_cgi_post);
		delete_client(this->_epoll_fd, ptr_cgi2->get_fd(), this->_fd_to_info, ptr_cgi2);
	}
}

#define RESET "\033[0m"
#define RED "\033[31m"

void Response::null_cgi( void) {
	this->_cgi_get = NULL;
	this->_cgi_post = NULL;
}

Response::Response(Request &request, Server &server, std::map<int, Client *> &fd_to_info, const int &epoll_fd, const int &client_fd, std::vector<Server *> &vec_server)
: _server(&server), _req(&request), _status_code(request.get_return_code()), _path(determine_final_path(request, server)), _http_type("HTTP/1.1"), _body(""), _type(request.get_type()), _cgi_started(false), _cgi_get(NULL), _cgi_post(NULL), _fd_to_info(fd_to_info), _epoll_fd(epoll_fd)
{
	// std::cout << "client fd : " << client_fd << " request path: " << request.get_target() << " final path : " << this->_path << " \n_status_code: " << request.get_return_code() << std::endl;
	this->_header["Server"] = "42WEBSERV";
	if (this->_status_code == 0)
		this->check_allowed_method(request.get_type(), server);


	if (this->_is_cgi(request, server))
	{
		this->_creat_envp(request);
		if (this->exec_cgi(fd_to_info, epoll_fd, client_fd, vec_server)) {
			return ;
		}
		this->_cgi_started = true;
		return;
	}
	//check if client max body size and implement return code accordingly
	// std::cout << "Path :" << this->_path << std::endl;
	if (this->_status_code == 0 && !server.get_inlocation_return().empty())
		this->set_status(301);
	// std::cout << "PAAATH " << this->_path << std::endl;
	if (this->_status_code == 0  && !request.get_type().compare("GET"))
		this->set_get_response();
	else if (this->_status_code == 0  && !request.get_type().compare("POST"))
		this->set_post_response(request);
	else if (this->_status_code == 0 && !request.get_type().compare("DELETE"))
		this->set_delete_response(request);
	if (this->_status_code == 0)
	{
		this->_status_code = 200;
		this->_reason_phrase = "OK";
	}
	else if (this->_status_code > 299 &&  this->_status_code < 399)
		this->set_redirect(server);
	else
		this->set_error_response(server);
	// std::cout << "-------response header ------------" << std::endl;
	// this->print_headers();
	// std::cout << "-------------------------" << std::endl;
}


Response&	Response::operator=(const Response& other)
{
	this->_status_code = other.get_status_code();
	this->_path = other.get_path();
	this->_http_type = other.get_http_type();
	this->_arguments = other.get_arguments();
	this->_reason_phrase = other.get_reason_phrase();
	this->_content_type = other.get_content_type();
	this->_header = other.get_headers();
	this->_body = other.get_body();
	this->_autoindex = other.get_autoindex();
	this->_cgi_started = other._cgi_started;
	return (*this);
}

const std::string& 							Response::get_path() const
{
	return (this->_path);
}

const std::string& 							Response::get_http_type() const
{
	return (this->_http_type);
}

const unsigned short int&					Response::get_status_code() const
{
	return (this->_status_code);
}

const std::string&							Response::get_arguments() const
{
	return (this->_arguments);
}

const std::string&							Response::get_reason_phrase() const
{
	return (this->_reason_phrase);
}

const std::string&							Response::get_content_type() const
{
	return (this->_content_type);
}

const std::string&							Response::get_body() const
{
	return (this->_body);
}

const std::map<std::string, std::string>&	Response::get_headers() const
{
	return (this->_header);
}

const bool&									Response::get_autoindex() const
{
	return (this->_autoindex);
}


const bool&									Response::get_cgi_status() const
{
	return (this->_cgi_started);
}


static std::string	set_full_path(Server& server, std::string& path)
{
	std::string	full_path;

	if (server.get_inlocation_root().empty() && !server.get_root().empty())
		full_path = reconstruct_path(server.get_root(), path);
	else if (!server.get_inlocation_root().empty())
		full_path = reconstruct_path(server.get_inlocation_root(), path.substr(server.get_inlocation_location().length()));
	else
		full_path = path;
	return (full_path);
}

static bool			check_path_permissions(std::string path, Request& request, Server& server)
{
	struct stat sfile;
// path.find_last_of('/') != std::string::npos ||
	// std::cout << "Original" << path << std::endl;
	// std::cout << "ROOT : " << server.get_root() << std::endl;
	while (path.compare(server.get_root()))
	{
		// std::cout << "Path :" << path << std::endl;
		size_t n = path.find_last_of('/');
		// std::cout << "n:" << n << std::endl;
		path.resize(n);
		// std::cout << "Path :" << path << std::endl;
		if (!stat(path.c_str(), &sfile))
		{
			if (!request.get_type().compare("GET") && !(sfile.st_mode & S_IROTH))
			{
				std::cout << "path : " << path << std::endl;
				return (1);
			}
			else if ((!request.get_type().compare("POST") || !request.get_type().compare("DELETE")) && !(sfile.st_mode & S_IWOTH))
				return (1);
		}
	}
	return (0);
}

static void			add_index(std::string& full_path, std::string index)
{
	if ((full_path.find_last_of('/') != full_path.length() - 1) && (index[0] != '/'))
		full_path += "/";
	else if ((full_path.find_last_of('/') == full_path.length() - 1) && (index[0] == '/'))
		full_path.erase(full_path.end() - 1);
	full_path += index;
}

const std::string	Response::determine_final_path(Request& request, Server& server)
{
	std::string		path;
	std::string		full_path;
	struct stat		sfile;

	if (request.get_body().size() > server.get_client_max_body_size())
		this->set_status(413);
	path = request.get_target().substr(0, request.get_target().find_first_of('?'));
	this->_autoindex = false;
	if (path.length() < request.get_target().length())
		this->_arguments = request.get_target().substr(request.get_target().find_first_of('?'));
	if ((this->_check_loc = server.check_location(path)))
	{
		full_path = set_full_path(server, path);
		// std::cout << "STATCODE|" << this->get_status_code() << std::endl;
		if (!stat(full_path.c_str(), &sfile) && S_ISDIR(sfile.st_mode))
		{
			// std::cout << "FULL PATH" << full_path << std::endl;
			if (!request.get_type().compare("GET"))
			{
				// std::cout << "L" << std::endl;
				if (!server.get_inlocation_index().empty())
				{
					// std::cout << "1" << std::endl;
					add_index(full_path, server.get_inlocation_index()[0]);
					// full_path += server.get_inlocation_index()[0];
				}
				else if (!server.get_index().empty())
				{

					// std::cout << "2" << std::endl;
					add_index(full_path, server.get_index()[0]);
					// full_path += server.get_index()[0];
				}
				else if (server.get_autoindex() == ON)
				{
					this->_autoindex = true;
					return (full_path);
				}
				else if (this->_autoindex == false)
				{
					this->set_status(403);
					return (full_path);
				}
			}
		}
		if (stat(full_path.c_str(), &sfile) < 0)
		{
			if (!stat(path.c_str(), &sfile) && !S_ISDIR(sfile.st_mode))
				return (path);
			if (!server.get_inlocation_return().empty())
				set_status(301);
			if (check_path_permissions(full_path, request, server))
			{
				// std::cout << "OUPS" << std::endl;
				set_status(403);
			}
			//CHECK IF FOLDER IS FORBIDDEN OR IF PATH DOES NOT EXIST

			set_status(404);
		}
		// std::cout << "request ret code: " << this->_status_code << std::endl;
		return (full_path);
	}
	else
	{
		set_status(404);
		return ("");
	}
}

void				Response::set_status(const unsigned short int& code)
{
	if (this->_status_code == 0)
		this->_status_code = code;
}

void				Response::set_error_response(Server& server)
{
	std::stringstream ss;
	this->_reason_phrase = reason_phrase(this->_status_code);
	this->fill_body_with_error_pages(server);
	if (this->_body.empty())
	{
		ss << "<html><body><h1>" << this->_status_code << " " << this->_reason_phrase << "</h1></body></html>";
		ss >> this->_body;
		this->_body += " " + this->_reason_phrase + "</h1></body></html>";
		this->_content_type = "text/html";
	}
	else
		this->_content_type = set_content_type(this->_path);
	this->set_error_headers();
}

void	Response::fill_body_with_error_pages(Server& server)
{
	std::ifstream				stream;
	std::string					path;
	std::string					error_line;
	std::vector<std::string>	error_vector;
	if (this->_check_loc && !(server.get_inlocation_error_page().empty()) \
		&& server.get_inlocation_error_page().find(this->_status_code) != server.get_inlocation_error_page().end())
	{
		std::cout << "here" << std::endl;
		if (!server.get_inlocation_root().empty())
			path = reconstruct_path(server.get_inlocation_root(), server.get_inlocation_error_page().find(this->_status_code)->second);
		else
			path = reconstruct_path(server.get_root(), server.get_inlocation_error_page().find(this->_status_code)->second);
		stream.open(path.c_str());
		if (stream.is_open())
		{
			this->_content_type = set_content_type(path);
			while (std::getline(stream, error_line))
				error_vector.push_back(error_line);
			for (std::vector<std::string>::iterator it = error_vector.begin(); it != error_vector.end(); it++)
			{
				if (it == error_vector.end() - 1)
					this->_body += *it;
				else
					this->_body += *it + "\n";
			}
			return ;
		}
	}
	else if (!(server.get_error_page().empty()) && (server.get_error_page().find(this->_status_code) != server.get_error_page().end()))
	{
		std::cout << "there" << std::endl;
		path = reconstruct_path(server.get_root(), server.get_error_page().find(this->_status_code)->second);
		stream.open(path.c_str());
		// std::cout << path << std::endl;
		if (stream.is_open())
		{
			this->_content_type = set_content_type(path);
			while (std::getline(stream, error_line))
				error_vector.push_back(error_line);
			for (std::vector<std::string>::iterator it = error_vector.begin(); it != error_vector.end(); it++)
			{
				if (it == error_vector.end() - 1)
					this->_body += *it;
				else
					this->_body += *it + "\n";
			}
		}
	}
}

std::string	Response::set_content_type(const std::string&	path)
{
	if (!path.empty() && path.find_last_of('.') != std::string::npos)
	{
		std::string ext = path.substr(path.find_last_of('.'));
		if (!ext.empty())
		{
			std::ifstream file;
			file.open("./server_files/mime_types.text");
			if (file.is_open())
			{
				std::string line;
				while (std::getline(file, line))
				{
					std::istringstream linestream(line);
					std::string word1;
					std::string word2;
					linestream >> word1;
					linestream >> word2;
					if (!word1.compare(ext))
						return (word2);
				}
			}
		}
	}
	return ("application/octet-stream");
}

static std::string	header405(Server& server, bool check_loc)
{
	std::string result = "Supported methods : ";
	if (check_loc)
	{
		for (std::vector<std::string>::const_iterator it = server.get_inlocation_allow_methods().begin(); it != server.get_inlocation_allow_methods().end(); it++)
		{
			result += *it;
			result += " ";
		}		
	}
	else {
		for (std::vector<std::string>::const_iterator it = server.get_allow_methods().begin(); it != server.get_allow_methods().end(); it++)
		{
			result += *it;
			result += " ";
		}		
	}
	return (result);
}

void	Response::set_error_headers()
{
	this->_header["Connection"] = "close";
	if (this->get_status_code() == 405)
		this->_header["Allow"] = header405(*this->_server, this->_check_loc);
	std::stringstream	ss;
	std::string			len;
	if (!this->_body.empty())
	{
		ss << this->_body.length();
		ss >> len;
		this->_header["Content-Length"] = len;
	}
}

static std::string		reason_phrase(unsigned short int& code)
{
	std::map<unsigned short int, std::string> httpErrorCodes;
    httpErrorCodes[100] = "Continue";
    httpErrorCodes[101] = "Switching Protocols";
    httpErrorCodes[200] = "OK";
    httpErrorCodes[201] = "Created";
    httpErrorCodes[202] = "Accepted";
    httpErrorCodes[204] = "No Content";
    httpErrorCodes[300] = "Multiple Choices";
    httpErrorCodes[301] = "Moved Permanently";
    httpErrorCodes[302] = "Found";
    httpErrorCodes[304] = "Not Modified";
    httpErrorCodes[400] = "Bad Request";
    httpErrorCodes[401] = "Unauthorized";
    httpErrorCodes[403] = "Forbidden";
    httpErrorCodes[404] = "Not Found";
    httpErrorCodes[405] = "Method Not Allowed";
    httpErrorCodes[408] = "Request Timeout";
    httpErrorCodes[413] = "Request Entity Too Large";
    httpErrorCodes[429] = "Too Many Requests";
    httpErrorCodes[500] = "Internal Server Error";
    httpErrorCodes[501] = "Not Implemented";
    httpErrorCodes[502] = "Bad Gateway";
    httpErrorCodes[503] = "Service Unavailable";
    httpErrorCodes[504] = "Gateway Timeout";
	return (httpErrorCodes[code]);
}

std::string	Response::construct_response(void)
{
	std::stringstream ss;

	// ss << this->get_http_type() << " " << this->get_status_code() << " " << this->get_reason_phrase();
	ss << this->get_http_type() << " ";
	ss << this->get_status_code() << " ";
	ss << this->get_reason_phrase();
	if (!(this->_header["Server"].empty()))
	{
		ss << "\r\n";
		for (std::map<std::string, std::string>::iterator it = this->_header.begin(); it != this->_header.end(); it++)
			ss << it->first << ": " << it->second << "\r\n";
	}
	ss << "\r\n";
	if (!(this->_body.empty())) {
		ss << this->_body;
		// std::cout << RED << this->_body.c_str() <<RESET<< std::endl; //comm--flo
	}
	return (ss.str());
}

void	Response::set_get_response()
{
	struct stat					sfile;
	// std::cout << "PATH|" << this->get_path() << "|" << std::endl;
	if (this->_autoindex == true && !stat(this->_path.c_str(), &sfile) && S_ISDIR(sfile.st_mode))
	{
		// std::cout << "here " << std::endl;
		std::time_t result = std::time(NULL);
		std::stringstream buffer;
		std::string time_str;
		buffer << result;
		buffer >> time_str;
		std::fstream tmp;
		tmp.open(time_str.c_str(), std::fstream::in | std::fstream::out | std::fstream::trunc);
		if (!tmp.is_open())
			this->set_status(500);
		else
		{
			tmp << "<html><body><h1> You are in a directory : \n</h1><p>";
			DIR*	dir = opendir(this->_path.c_str());
			struct dirent* entry;
			while ((entry = readdir(dir)) != NULL)
				tmp << entry->d_name << "\n";
			tmp << "</p></body></html>";
			closedir(dir);
		}
		tmp.seekg(0, std::ios::beg);
		std::string		line;
		while (std::getline(tmp, line))
			this->_body += line + "<br>";
		// std::cout << "bodyyy" << this->_body << std::endl;
		this->set_get_headers();
		tmp.close();
		std::remove(time_str.c_str());
	}
	else if (!stat(this->_path.c_str(), &sfile) && (sfile.st_mode & S_IROTH))
	{
		// std::cout << RED << "je suis path.c_str(): " << this->_path.c_str() <<RESET<< std::endl; //comm--flo
		std::ifstream				file;
		std::string					line;
		std::vector<std::string>	all_lines;

		file.open(this->_path.c_str());
		if (!file.is_open())
			set_status(500);
		else
		{
			while (std::getline(file, line))
				all_lines.push_back(line);
			for (std::vector<std::string>::iterator it = all_lines.begin(); it != all_lines.end(); it++)
				this->_body += *it + "\n";
			this->set_get_headers();
		}
	}
	else
		set_status(404);
}

// static void	delete_cr(std::string& line)
// {
// 	if (line.find_first_of('\r') != std::string::npos && line.find_first_of('\r') == line.size() - 1)
// 		line.erase(line.find_first_of('\r'));
// }

static bool fileExists(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    return file.good();
}

static std::string							get_buff_line(std::vector<char>& buff)
{
	std::string	return_value = "";
	std::vector<char>::iterator it = buff.begin();

	while (it != buff.end())
	{
		if (*it == '\r' && *(it + 1) && *(it + 1) == '\n')
		{
			buff.erase(it);
			buff.erase(it);
			return (return_value);
		}
		return_value += *it;
		buff.erase(it);
	}
	return (return_value);
}

static std::vector<char>::iterator	find_iterator(std::vector<char>& buff, std::string& separator)
{
	std::string result;

	std::vector<char>::iterator it = buff.begin();
	while (it != buff.end())
	{
		if (result.find(separator) != std::string::npos)
			break ;
		result += *it;
		it++;
	}
	if (result.find(separator) == std::string::npos)
		return (it);
	std::string result2;
	// std::cout << "it|" << *it << "|" << std::endl;
	while (it != buff.begin() && (result2.find(separator) == std::string::npos))
	{
		// std::cout << "Result|" << result << "|" << std::endl;
		result2.insert(result2.begin(), *it);
		it--;
		// std::cout << *it << std::ends;
	}
	// std::cout << "SEP|" << separator << "|" << std::endl;
	// std::cout << "Result|" << result2 << "|" << std::endl;
	while (it != buff.begin() && *it == '-')
			it--;
	if (it != buff.begin())
		it--;
	return (it);
}

void	Response::open_file(std::ofstream& file, std::vector<char>& buff)
{
	std::string line = get_buff_line(buff);
	if (line.find("filename=") == std::string::npos)
	{
		this->set_status(400);
		for (std::vector<char>::iterator it = buff.begin(); it != buff.end(); it++)
			std::cout << *it << std::ends;
		std::cout << std::endl;
		return ;
	}
	std::string filename = line.substr(line.find("filename="));
	filename = filename.substr(filename.find_first_of("\"") + 1);
	filename = filename.substr(0, filename.find_last_of("\""));
	if (filename.empty())
		this->set_status(400);
	std::string topen = this->_path + "/" + filename;
	size_t i = 1;
	std::string base = topen;
	while (1)
	{
		if (!fileExists(topen))
		{
			file.open(topen.c_str());
			if (file.is_open())
				break ;
		}
		std::stringstream plus;
		plus << "(" << i << ")";
		std::string myplus;
		plus >> myplus;
		std::string newbase = base;
		if (newbase.find_last_of('.') != std::string::npos)
			topen = newbase.insert(newbase.find_last_of('.'), myplus);
		else
			topen = newbase + myplus;
		i++;
		if (i == 65535)
		{
			this->set_status(500);
			break ;
		}
	}
}

void	Response::set_post_response(Request& request)
{
	struct stat					sfile;
	if (!stat(this->_path.c_str(), &sfile) && S_ISDIR(sfile.st_mode) && (sfile.st_mode & S_IWOTH))
	{
		std::string type = request.get_header("Content-Type");
		std::vector<char> buff = request.get_body();
		if ((type.find_first_of(';') == std::string::npos) || type.find("boundary=") == std::string::npos || type.substr(0, type.find_first_of(';')).compare("multipart/form-data"))
			this->set_status(400);
		std::string separator;
		if (type.find(" boundary=") != std::string::npos)
			separator = type.substr(type.find(" boundary=") + 10);
		else
			this->set_status(400);
		std::string line = get_buff_line(buff);
		if (line.find_first_not_of('-') != std::string::npos)
			line = line.substr(line.find_first_not_of('-'));
		
		std::string sep2;
		if (separator.find_first_not_of('-') != std::string::npos)
		{
			sep2 = separator.substr(separator.find_first_not_of('-'), separator.find_last_not_of('-'));
			if (line.compare(sep2))
				this->set_status(400);
		}
		// request.print_headers();
		// std::cout << "|";
		// request.print_body();
		// std::cout << "|" << std::endl;
		std::ofstream file;
		open_file(file, buff);
		do {
			line = get_buff_line(buff);
			// std::cout << "|" << line << "|" << std::endl;
		}
		while (line.compare(""));
		// for (std::vector<char>::iterator it = buff.begin(); it != buff.end(); it++)
		// 	std::cout << *it << std::ends;
		// std::cout << "ENDHERE"<< std::endl;
		std::vector<char>::iterator limit = find_iterator(buff, separator);
		if (file.is_open())
		{
			for (std::vector<char>::iterator it = buff.begin(); it != limit; it++)
				file << *it;
		}
		// std::cout << "SUCCESS" << std::endl;
		if (this->_status_code == 0)
		{
			this->_body.append("File well uploaded");
			this->set_post_headers();
		}
	}
	else if (!stat(this->_path.c_str(), &sfile) && (sfile.st_mode & S_IWOTH))
	{
		std::fstream				file;

		file.open(this->_path.c_str(), std::fstream::out | std::fstream::app);
		if (!file.is_open())
			set_status(500);
		else
		{
			std::stringstream 	ss;
			std::vector<char>	requestbody = request.get_body();
			for (std::vector<char>::iterator it = requestbody.begin(); it != requestbody.end(); it++)
				ss << *it;
			file << ss.str() << "\n";
			this->_body.append("Message well received");
			this->set_post_headers();
		}
	}
	else
		set_status(403);
}

void	Response::set_delete_response(Request& request)
{
	struct stat					sfile;
	if (!stat(this->_path.c_str(), &sfile) && (sfile.st_mode & S_IWOTH))
	{
		if (std::remove(this->_path.c_str()))
			set_status(500);
		else
		{
			this->_body = "<html><body><h1>";
			this->_body += request.get_target();
			this->_body += " deleted </h1></body></html>";
		}
		set_delete_headers();
	}
	else
		set_status(403);
}

void				Response::set_redirect(Server& server)
{
	this->_body = "<html><head><title>301 Moved Permanently</title></head><body><center><h1>301 Moved Permanently</h1></center><hr><center>42WEBSERV</center></body></html>";
	this->_header["Content-Type"] = "text/html";
	this->_header["Content-Length"] = this->_body.length();
	this->_header["Connection"] = "keep-alive";
	this->_header["Location"] = server.get_inlocation_return();
}

void	Response::set_delete_headers()
{
	this->_header["Content-Type"] = "text/plain";
	this->_header["Connection"] = "Keep-alive";
	std::stringstream ss;
	std::string			len;
	if (!this->_body.empty())
	{
		ss << this->_body.length();
		ss >> len;
		this->_header["Content-Length"] = len;
	}
}

void	Response::set_post_headers()
{
	this->_header["Content-Type"] = "text/plain";
	this->_header["Connection"] = "Keep-alive";
	this->_header["Cache-Control"] = "no-cache";
	std::stringstream ss;
	std::string			len;
	if (!this->_body.empty())
	{
		ss << this->_body.length();
		ss >> len;
		this->_header["Content-Length"] = len;
	}
}

void	Response::set_cgi_headers()
{
	this->_header["Connection"] = "Keep-alive";
	std::stringstream ss;
	std::string			len;
	if (!this->_body.empty())
	{
		ss << this->_body.length();
		ss >> len;
		this->_header["Content-Length"] = len;
	}
}

void	Response::set_get_headers()
{
	if (this->_autoindex == true)
		this->_content_type = "text/html";
	else
		this->_content_type = set_content_type(this->_path);
	this->_header["Content-Type"] = this->_content_type;
	this->_header["Connection"] = "Keep-alive";
	std::stringstream ss;
	std::string			len;
	if (!this->_body.empty())
	{
		ss << this->_body.length();
		ss >> len;
		this->_header["Content-Length"] = len;
	}
}

const std::string&	Response::get_connection_header() const
{
	return (this->_header.find("Connection")->second);
}

static std::string	reconstruct_path(std::string s1, std::string s2)
{
	return (s1 + s2);
}


void				Response::check_allowed_method(const std::string& _method_requested, Server& server)
{
	std::string	methods[9] = {"GET", "HEAD", "OPTIONS", "TRACE", "PUT", "DELETE", "POST", "PATCH", "CONNECT"};
	std::string implemented_methods[3] = {"GET", "POST", "DELETE"};

	for (int i = 0; i < 9; i++)
	{
		if (!_method_requested.compare(methods[i]))
		{
			for (int j = 0; j < 3; j++)
			{
				if (!_method_requested.compare(implemented_methods[j]))
					break ;
				if (j == 2)
					this->set_status(501);
			}
			break ;
		}
		if (i == 8)
			this->set_status(400);
	}
	if (server.get_inlocation_allow_methods().size() != 0)
	{
		for (size_t i = 0; i < server.get_inlocation_allow_methods().size(); i++)
		{
			if (!_method_requested.compare(server.get_inlocation_allow_methods()[i]))
				return ;
		}
		this->set_status(405);
	}
	else if (server.get_allow_methods().size() != 0)
	{
		for (size_t i = 0; i < server.get_allow_methods().size(); i++)
		{
			if (!_method_requested.compare(server.get_allow_methods()[i]))
				return ;
		}
		this->set_status(405);
	}
}


/*--------- CGI ---------*/

void 		Response::print_headers() const {

	for (std::map<std::string, std::string>::const_iterator it = this->_header.begin(); it != this->_header.end(); it++) {

		std::cout << it->first << "=" << it->second << std::endl;
	}
}

static std::string normalize_header_cgi_metavariable(const std::string& header_name) {
	std::string normalized_name = "HTTP_";

	// 1. Convert caps and underscore.
	for (size_t i = 0; i < header_name.size(); i++) {
		char c = header_name[i];
		if (c == '-') {
			normalized_name += '_';
		} else {
			normalized_name += std::toupper(static_cast<unsigned char>(c));
		}
	}

	// 2. For content -> type and lenght do not change.
	if (normalized_name == "HTTP_CONTENT_TYPE") {
		return "CONTENT_TYPE";
	}
	if (normalized_name == "HTTP_CONTENT_LENGTH") {
		return "CONTENT_LENGTH";
	}

	/*
	QUERY_SRING
	*/

	//3. More special case.

	return (normalized_name);
}

void			Response::_creat_envp(Request &req) {

	const std::map<std::string, std::string> ptr = req.get_headers();

	this->_envp.push_back(std::string("SERVER_SOFTWARE=WEBSERVE/1.0"));
	this->_envp.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
	this->_envp.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
	for (std::map<std::string, std::string>::const_iterator it = ptr.begin();
		it != ptr.end(); it++) {

		std::string str = normalize_header_cgi_metavariable(it->first) + "=" + it->second;
		this->_envp.push_back(str);
	}
	this->_envp.push_back(std::string("REQUEST_METHOD=" + req.get_type()));
	this->_envp.push_back(std::string("QUERY_SRING="+ this->get_arguments()));
	this->_envp.push_back(std::string("PATH_TRANSLATED="+ this->get_path()));// /home/fcretin/project/webserv/server_files/cgi/cgi-bash/
	this->_envp.push_back(std::string("PATH_INFO=" + req.get_target().substr(0, req.get_target().find_last_of('/'))));// /cgi/cgi-bash/
	this->_script_name = req.get_target().substr(req.get_target().find_last_of('/') + 1, req.get_target().size());
	this->_envp.push_back(std::string("SCRIPT_NAME=" +  this->_script_name)); // data_heure.sh
}

std::vector<const char *> Response::_extrac_envp( void ) {

	std::vector<const char *> vec_char;

	for (size_t i = 0; i < this->_envp.size(); i++)
	{
		vec_char.push_back(this->_envp[i].c_str());
	}
	return vec_char;
}


#include "ClientCgi.hpp"
std::string		Response::construct_response_cgi(void) {
	this->set_cgi_headers();
	if (this->_status_code == 0)
	{
		this->_status_code = 200;
		this->_reason_phrase = "OK";
	}
	else if (this->_status_code > 299 &&  this->_status_code < 399)
		this->set_redirect(*this->_server);
	else
		this->set_error_response(*this->_server);
	return (this->construct_response());
}
void				Response::set_body(const std::string &str) {
	this->_body = str;
}

bool				Response::_is_cgi(Request& request, Server& server) {

	std::string path = request.get_target().substr(0, request.get_target().find_first_of('?'));
	if (path.empty() || path.find('.') == std::string::npos) {
		return false;
	}
	if (!server.check_location(path)) {
		return false;
	}
	std::string extension = path.substr(path.find('.'), path.length());
	std::size_t cgi_extension_pos = server.get_inlocation_cgi_extension().find(extension);
	if (cgi_extension_pos == std::string::npos) {
		return false;
	}
	if (server.get_inlocation_cgi_extension()[cgi_extension_pos + extension.length()] != '\0') {
		return false;
	}
	std::size_t cgi_path_pos = server.get_inlocation_cgi_path().find(extension);
	if	(cgi_path_pos == std::string::npos) {
		return false;
	}
	if	(server.get_inlocation_cgi_path()[cgi_path_pos + extension.length()] != ':') {
		return false;
	}
	if (extension != server.get_inlocation_cgi_path().substr(0, server.get_inlocation_cgi_path().find(':'))) {
		return false;
	}
	this->_path_cgi = server.get_inlocation_cgi_path().substr(server.get_inlocation_cgi_path().find(':') + 1 , server.get_inlocation_cgi_path().size());
	if (this->_path_cgi.empty()) {
		return false;
	}
	return true;
}

int				Response::exec_cgi(std::map<int, Client *> &fd_to_info, const int &epoll_fd, const int &client_fd, std::vector<Server *> &vec_server) {

	std::vector<const char *> vec_char = this->_extrac_envp();
	vec_char.push_back(NULL);

	std::vector<const char *> vec_arg;
	vec_arg.push_back(this->_path_cgi.c_str());
	vec_arg.push_back(this->_path.c_str());
	vec_arg.push_back(NULL);

	if (this->cgi(this->_path_cgi.c_str(), vec_arg.data(), vec_char.data(), fd_to_info, epoll_fd, client_fd, vec_server)) {
		return (-1);
	}
	return (0);
}

#define MAX_BUFFER			1048

#define YELLOW "\033[33m"
#include <sys/wait.h>
#include <cstring>
#include <sys/epoll.h>
#include "ClientCgi.hpp"
// maybe cookie...
// set-cookie="qwerq=qwe"
// Cookie=qwe
int				Response::cgi(const char *path, const char **script, const char **envp, std::map<int, Client *> &fd_to_info, const int &epoll_fd, const int &client_fd, std::vector<Server *> &vec_server) {

	pid_t	pid;
	int		pipe_in[2];
	int		pipe_out[2];
	const bool	second_pipe = this->_type == "POST";

	if (pipe(pipe_out) == -1) {

		close(pipe_out[0]);
		close(pipe_out[1]);
		return (-1);
	}
	if (second_pipe) {
		//only post
		if (pipe(pipe_in) == -1) {

			close(pipe_out[0]);
			close(pipe_out[1]);
			close(pipe_in[0]);
			close(pipe_in[1]);
			return (-1);
		}
	}
	pid = fork();
	// std::cout << "pid = fork(): " << pid <<" \n";
	if (pid == -1) {

		close(pipe_out[0]);
		close(pipe_out[1]);
		if (second_pipe) {

			std::cout << "error3\n";
			close(pipe_in[0]);
			close(pipe_in[1]);
		}
		return (-1);

	} else if (pid == 0) { //child

		dup2(pipe_out[1], 1);
		close(pipe_out[0]);
		close(pipe_out[1]);
		if (second_pipe) {

			dup2(pipe_in[0], 0);
			close(pipe_in[0]);
			close(pipe_in[1]);
		}

		clean_fd(fd_to_info, epoll_fd, vec_server);
		execve(path, const_cast<char *const *>(script), const_cast<char *const *>(envp));
		std::cerr << "Error: Execve failed." << std::endl;
		clean_for_cgi(fd_to_info, vec_server);

		throw (-42);

	} else {

		close(pipe_out[1]);
		if (second_pipe) {
			close(pipe_in[0]);
		}

		Client * ptr1 = new ClientCgi(-1, pipe_out[0], client_fd);
		if (!epollctl(epoll_fd, pipe_out[0], EPOLLIN, EPOLL_CTL_ADD)) {
			// std::cout << "error\n";
			close(pipe_out[0]);
			delete ptr1;
			return (-1);
		}

		ClientCgi *ptr_cgi1 = dynamic_cast<ClientCgi *>(ptr1);
		ptr_cgi1->set_response(this);
		ptr_cgi1->set_pid(pid);


		if (second_pipe) {

			Client *ptr2 = new ClientCgi(pipe_in[1], -1, client_fd);// client fd pas besoin
			if (!epollctl(epoll_fd, pipe_in[1], EPOLLOUT, EPOLL_CTL_ADD)) {
				// std::cout << "error2\n";
				close(pipe_out[0]);
				delete ptr2;
				delete ptr1;
				return (-1);
			}


			ClientCgi *ptr_cgi2 = dynamic_cast<ClientCgi *>(ptr2);
			ptr_cgi2->add_body_request(this->_req->get_body());
			ptr_cgi2->set_pid(pid);
			this->_cgi_post = ptr2;
			fd_to_info[pipe_in[1]] = ptr2;
		}

		this->_cgi_get = ptr1;
		fd_to_info[pipe_out[0]] = ptr1;

	}
	return (0);
}
