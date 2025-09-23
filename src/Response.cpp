/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sithomas <sithomas@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 14:43:37 by sithomas          #+#    #+#             */
/*   Updated: 2025/09/10 18:25:09 by sithomas         ###   ########.fr       */
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

// enum http_types {
//     CR = '\r',
//     LF = '\n',
//     SP = ' ',
// 	HTAB = '\t'
// } t_http_types;

static std::string		reason_phrase(unsigned short int& code);
static std::string		reconstruct_path(std::string s1, std::string s2);

Response::~Response()
{
}

Response::Response(Request& request, Server& server)
: _status_code(request.get_return_code()), _path(determine_final_path(request, server)), _http_type("HTTP/1.1")
{
	this->_header["Server"] = "42WEBSERV";
	// std::cout << "request ret code: " << this->_status_code << std::endl;
	//check if client max body size and implement return code accordingly
	if (this->_status_code == 0)
		this->check_allowed_method(request.get_type(), server);
	if (this->_status_code == 0 && !server.get_inlocation_return().empty())
		this->set_status(301);
	// std::cout << "request ret code: " << this->_status_code << std::endl;
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

const std::string	Response::determine_final_path(Request& request, Server& server)
{
	std::string		path;
	std::string		full_path;
	struct stat		sfile;

	path = request.get_target().substr(0, request.get_target().find_first_of('?'));
	this->_autoindex = false;
	if (path.length() < request.get_target().length())
		this->_arguments = request.get_target().substr(request.get_target().find_first_of('?'));
	if (server.check_location(path))
	{
		full_path = set_full_path(server, path);
		if (!stat(full_path.c_str(), &sfile) && S_ISDIR(sfile.st_mode))
		{
			if (!request.get_type().compare("GET"))
			{
				if (!server.get_inlocation_index().empty())
					full_path += server.get_inlocation_index()[0];
				else if (!server.get_index().empty())
					full_path += server.get_index()[0];
				else if (server.get_autoindex() == ON)
				{
					this->_autoindex = true;
					return (full_path);
				}
			}
		}
		if (stat(full_path.c_str(), &sfile) < 0)
		{
			if (!stat(path.c_str(), &sfile) && !S_ISDIR(sfile.st_mode))
				return (path);
			set_status(404);
		}
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
	}
	this->set_error_headers();
}

void	Response::fill_body_with_error_pages(Server& server)
{
	std::ifstream				stream;
	std::string					path;
	std::string					error_line;
	std::vector<std::string>	error_vector;
	if (server.check_location(this->_path) && !(server.get_inlocation_error_page().empty()) \
		&& server.get_inlocation_error_page().find(this->_status_code) != server.get_inlocation_error_page().end())
	{
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

void	Response::set_error_headers()
{
	this->_header["Content-Type"] = this->_content_type;
	this->_header["Connection"] = "close";
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
    httpErrorCodes[429] = "Too Many Requests";
    httpErrorCodes[500] = "Internal Server Error";
    httpErrorCodes[501] = "Not Implemented";
    httpErrorCodes[502] = "Bad Gateway";
    httpErrorCodes[503] = "Service Unavailable";
    httpErrorCodes[504] = "Gateway Timeout";
	return (httpErrorCodes[code]);
}

void	Response::write_response(int& client_fd)
{
	std::string	response;
	std::stringstream ss;

	ss << this->_http_type << " " << this->_status_code << " " << this->_reason_phrase;
	if (!(this->_header["Server"].empty()))
	{
		ss << "\r\n";
		for (std::map<std::string, std::string>::iterator it = this->_header.begin(); it != this->_header.end(); it++)
			ss << it->first << ": " << it->second << "\r\n";
	}
	ss << "\r\n";
	if (!(this->_body.empty()))
		ss << this->_body;
	response = ss.str();
	send(client_fd, response.c_str(), response.length(), MSG_DONTWAIT);
}

void	Response::set_get_response()
{
	struct stat					sfile;
	if (this->_autoindex == true)
	{
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
		set_status(403);
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
		std::cout << *it << std::ends;
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
		//to DEAL  EXCEPTION BAD REQUEST;
		std::cout << "filename not found : line|" << line << "|" << std::endl;
		return ;
	}
	std::string filename = line.substr(line.find("filename="));
	// std::cout << "FILENAME " << filename << "|" << "SIZE " << filename.size() << std::endl;
	filename = filename.substr(filename.find_first_of("\"") + 1);
	filename = filename.substr(0, filename.find_last_of("\""));
	if (filename.empty())
	{
		//to DEAL  EXCEPTION BAD REQUEST;
		std::cout << "filename empty" << std::endl;
	}
	// std::cout << "FILENAME 2|" << filename << "|" << std::endl;
	// std::cout << this->_path << std::endl;
	std::string topen = this->_path + "/" + filename;
	size_t i = 1;
	std::string base = topen;
	while (1)
	{
		// std::cout << "Nous y voila" << std::endl;
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
			this->set_status(502);
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
		line = line.substr(line.find_first_not_of('-'));
		std::string sep2 = separator.substr(separator.find_first_not_of('-'), separator.find_last_not_of('-'));
		if (line.compare(sep2))
			this->set_status(400);
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
