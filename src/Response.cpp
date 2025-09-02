/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sithomas <sithomas@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 14:43:37 by sithomas          #+#    #+#             */
/*   Updated: 2025/08/28 15:49:28 by sithomas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

static std::string		reason_phrase(unsigned short int& code);
static std::string		reconstruct_path(std::string s1, std::string s2);

Response::~Response()
{
}

Response::Response(Request& request, Server& server)
: _status_code(request.get_return_code()), _path(determine_final_path(request, server)), _http_type("HTTP/1.1")
{
	this->_header["Server"] = "42WEBSERV";
	//check if client max body size and implement return code accordingly
	if (this->_status_code == 0  && !request.get_type().compare("GET"))
		this->set_get_response();
	else if (this->_status_code == 0  && !request.get_type().compare("POST"))
		this->set_post_response(request);
	if (this->_status_code == 0)
	{
		this->_status_code = 200;
		this->_reason_phrase = "OK";
	}
	else if (this->_status_code > 199 )
		this->set_error_response(server);
}

static std::string	set_full_path(Server& server, std::string& path)
{
	std::string	full_path;

	if (server.get_inlocation_root().empty() && !server.get_root().empty())
		full_path = reconstruct_path(server.get_root(), path);
	else if (!server.get_inlocation_root().empty())
		// full_path = reconstruct_path(server.get_inlocation_root(), path.substr(server.get_locatio ().length()));
		full_path = reconstruct_path(server.get_inlocation_root(), path);
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
	if (path.length() < request.get_target().length())
		this->_arguments = request.get_target().substr(request.get_target().find_first_of('?'));
	if (server.check_location(path))
	{
		full_path = set_full_path(server, path);
		if (!stat(full_path.c_str(), &sfile) && S_ISDIR(sfile.st_mode))
		{
			this->_isdir = true;
			if (!request.get_type().compare("GET"))
			{
				if (!server.get_inlocation_index().empty())
					full_path += server.get_inlocation_index()[0];
				else if (!server.get_index().empty())
					full_path += server.get_index()[0];
			}
		}
		if (stat(full_path.c_str(), &sfile) < 0)
		{
			if (!stat(path.c_str(), &sfile) && !S_ISDIR(sfile.st_mode))
				return (path);
			set_status(404);
		}
		this->_isdir = false;
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
		std::cout << path << std::endl;
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
			file.open("./server_files/mime_types.txt");
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
	write(client_fd, response.c_str(), response.length());
}

void	Response::set_get_response()
{
	struct stat					sfile;
	if (!stat(this->_path.c_str(), &sfile) && (sfile.st_mode & S_IROTH))
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
			set_get_headers();
		}
	}
	else
		set_status(401);
}

void	Response::set_post_response(Request& request)
{
	struct stat					sfile;
	if (!stat(this->_path.c_str(), &sfile) && (sfile.st_mode & S_IWOTH))
	{
		std::fstream				file;

		file.open(this->_path.c_str(), std::fstream::out | std::fstream::app);
		if (!file.is_open())
			set_status(500);
		else
		{
			std::stringstream ss;
			std::vector<char>	requestbody = request.get_body();
			for (std::vector<char>::iterator it = requestbody.begin(); it != requestbody.end(); it++)
				ss << *it;
			file << ss.str() << "\n";
			this->_body.append("Message well received");
			this->set_post_headers();
			// set_status(200);
		}
	}
	else
		set_status(401);
}

// void	Response::treat_post(Request& request)
// {
// 	if (!request.get_content_type().compare("application/x-www-urlencoded"))
// 	{

// 	}
// 	// else if (!request.get_content_type().compare("application/json"))
// 	// {}
// 	if (!request.get_content_type().compare("application/octet-stream"))
// 	{
// 		if (this->_isdir == false)
// 		{
// 			//error ?
// 		}
// 		else
// 		{

// 		}
// 	}
// 	else
// 	{
// 		//set_status(400); a verifier
// 	}

// }

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
