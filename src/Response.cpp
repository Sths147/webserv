/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sithomas <sithomas@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 14:43:37 by sithomas          #+#    #+#             */
/*   Updated: 2025/08/27 14:22:23 by sithomas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sys/stat.h>
#include <unistd.h>

static std::string		reason_phrase(unsigned short int& code);

Response::~Response()
{
}

Response::Response(Request& request, Server& server)		//builds get response
: _status_code(request.get_return_code()), _path(determine_final_path(request, server)), _http_type("HTTP/1.1")
{
	//check if client max body size and implement return code accordingly
	std::cout << "Here we are with a final path of" << this->_path << std::endl;
	if (this->_status_code != 0)
		this->set_error_response();
	else if (!request.get_type().compare("GET"))
		this->set_get_response();
	else if (!request.get_type().compare("POST"))
	{
		// this->set_post_response();
	}
	if (this->_status_code == 0)
	{
		this->_status_code = 200;
		this->_reason_phrase = "OK";
	}
	else
		this->set_error_response();
	// if (this->_status_code != 0)
	// 	this->set_error_response();
	// else
	// 	this->_status_code = 200;
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
		if (server.get_inlocation_root().empty() && !server.get_root().empty())
		{
			//must change this shit to change root into an alias
			full_path = server.get_root() + path;
		}
		else
			full_path = server.get_inlocation_root() + path;
		if (stat(full_path.c_str(), &sfile) < 0)
		{
			std::cout << "NOT EXIST" << std::endl;
			set_status(404);
		}
		else if (S_ISDIR(sfile.st_mode))
		{
			std::cout << "DIRECTORY" << std::endl;
		}
		else
		{
			std::cout << "IS OK " << std::endl;
		}
		return (full_path);
	}
	//if path est accessible directement alors on le return else on return ""
	else if (stat(path.c_str(), &sfile))
		return (path);
	else
		return ("");
}

void				Response::set_status(const unsigned short int& code)
{
	if (this->_status_code == 0)
		this->_status_code = code;
}

void				Response::set_error_response()
{
	this->_reason_phrase = reason_phrase(this->_status_code);
	std::stringstream ss;
	//if error pages for this number are set up, write the body else
	ss << "<html><body><h1>" << this->_status_code << " " << this->_reason_phrase << "</h1></body></html>";
	ss >> this->_body;
	// this->set_error_header();
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
		std::cout << "CAN READ" << std::endl;
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

void	Response::set_get_headers()
{
	this->_header["Content-Type"] = "text/html";
	this->_header["Connection"] = "Keep-alive";
	this->_header["Server"] = "VVVVVVVVVVVVVVV";
	std::stringstream ss;
	std::string			len;
	if (!this->_body.empty())
	{
		ss << this->_body.length();
		ss >> len;
		this->_header["Content-Length"] = len;
	}
}
