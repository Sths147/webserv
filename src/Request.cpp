/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sithomas <sithomas@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:22:58 by sithomas          #+#    #+#             */
/*   Updated: 2025/09/10 14:40:01 by sithomas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <limits>

enum http_types {
    CR = '\r',
    LF = '\n',
    SP = ' ',
	HTAB = '\t'
} t_http_types;

unsigned int	Request::_max_size = std::numeric_limits<uint>::max();

// Request::Request()
// : _type("NOTYPE")
// {
// }

Request::Request(std::vector<char>&buff)
: _return_code(0), _type(parse_request_type(buff)), _target(parse_request_target(buff)), _http_type(parse_http_type(buff)), _header(parse_header(buff)), _body(buff)
{
	for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); it++)
	{
		std::cout << it->first << "--" << it->second << std::endl;
	}
	std::string	mybody(this->_body.begin(), this->_body.end());
	std::cout << "\n\n and here is the body" << mybody << std::endl;
	this->parse_headers();
	// std::cout << "\n\n here we have a return code of" << this->get_return_code() << std::endl;
}

Request::~Request()
{
}

const std::string	Request::get_type() const
{
	return (this->_type);
}

const std::string	Request::get_target() const
{
	return (this->_target);
}

const std::string	Request::get_http_version() const
{
	return (this->_http_type);
}

unsigned short int	Request::get_return_code() const
{
	return (this->_return_code);
}

void	Request::set_return_code(const unsigned short int& code)
{
	if (this->_return_code == 0)
		this->_return_code = code;
}

void	Request::parse_headers()
{
	if (this->_header["Host"].empty())
		this->set_return_code(400);
}

/*
A server that receives a method longer than any that it implements SHOULD respond with a 501 (Not Implemented) status code. A server that receives a request-target longer than any URI it wishes to parse MUST respond with a 414 (URI Too Long) status code (see Section 15.5.15 of [HTTP]).
*/

const std::string	Request::parse_request_target(std::vector<char>& buff)
{
	std::string					result;
	(void)buff;
	std::vector<char>::iterator	k = buff.begin();
	if (*k != SP)
		set_return_code(400);
	buff.erase(k);
	while (k != buff.end())
	{
		if (*k == SP)
			break;
		else if (*k < 33 || *k > 126)
			set_return_code(400);
		else
		{
			result += *k;
			buff.erase(k);
		}
	}
	if (result.empty() || k == buff.end())
		set_return_code(400);
	return (result);
}

const std::string	Request::parse_http_type(std::vector<char>& buff)
{
	std::string 				result;
	const std::string			to_compare = "HTTP/1.1";
	std::vector<char>::iterator	k = buff.begin();

	if (*k != SP)
		set_return_code(400);
	buff.erase(k);
	for (int i = 0; i < 8; i++)
	{
		if (k == buff.end())
			set_return_code(400);
		if (i == 5 || i == 7)
		{
			if (!isdigit(*k))
				set_return_code(400);
		}
		else if (*k != to_compare[i])
			set_return_code(400);
		result += *k;
		buff.erase(k);
	}
	if (*k == CR)
		buff.erase(k);
	if (*k == LF)
	{
		buff.erase(k);
		return (result);
	}
	set_return_code(400);
	return (result);
}

static void			skip_crlf(std::vector<char>& buff)
{
	std::vector<char>::iterator	k = buff.begin();
	while (1)
	{
		if (*k == CR && *(k + 1) && *(k + 1) == LF)
		{
			buff.erase(k);
			buff.erase(k);
		}
		else if (*k == LF)
			buff.erase(k);
		else
			break ;
	}
}

const std::string	Request::parse_request_type(std::vector<char>& buff)
{
	std::string result;
	std::vector<char>::iterator k = buff.begin();
	skip_crlf(buff);
	for (int i = 0; i < 10; i++)
	{
		if (i == 9 || k == buff.end())
			set_return_code(400);
		else if (*k == SP)
			break;
		else if (*k >= 'A' && *k <= 'Z')
		{
			result += *k;
			buff.erase(k);
		}
		else
			set_return_code(400);
	}
	if (result.empty())
		set_return_code(400);
	// if (result.compare("GET") && result.compare("POST"))
	// 	set_return_code(400);
	return (result);
}

std::map<std::string, std::string>	Request::parse_header(std::vector<char>& buff)
{
	std::map<std::string, std::string>	result;
	std::string							line = get_crlf_line(buff);
	std::string							key;
	while (line.length() != 0)
	{
		key = parse_key(line);
		if (!result[key].empty())
			set_return_code(400);
		const std::string OWS = " \t";
		if (line.find_first_not_of(OWS) > line.find_last_not_of(OWS))
			set_return_code(400);
		line = line.substr(line.find_first_not_of(OWS), line.find_last_not_of(OWS) + 1);
		if (line.length() == 0)
			set_return_code(400);
		result[key] = line;
		line = get_crlf_line(buff);
	}
	return (result);
}

const std::string							Request::get_crlf_line(std::vector<char>& buff)
{
	std::string	return_value = "\0";
	std::vector<char>::iterator	k = buff.begin();

	while (k != buff.end())
	{
		if (*k == CR && *(k + 1) && *(k + 1) == LF)
		{
			buff.erase(k);
			buff.erase(k);
			return (return_value);
		}
		if (*k == LF)
		{
			buff.erase(k);
			return (return_value);
		}
		if (*k < 32 || *k > 126)
			set_return_code(400);
		return_value += *k;
		buff.erase(k);
	}
	set_return_code(400);
	return (return_value);
}

const std::string							Request::parse_key(std::string& line)
{
	std::string	return_value;
	size_t 		n = line.find(':');

	if (n == std::string::npos)
		set_return_code(400);
	else if (n == 0)
		set_return_code(400);
	else if (n == line.length() - 1)
		set_return_code(400);
	else
	{
		return_value = line.substr(0, n);
		line = line.substr(n + 1);
	}
	return (return_value);
}
// Listen	Request::set_listen()
// {
// 	for (std::map<std::string, std::string>::iterator it = this->_header.begin(); it != this->_header.end(); it++)
// 	{
// 		if (it->first == "Host")
// 		{
// 			try {
// 				Listen result = ConfigUtils::ip_host_parseur(it->second);
// 				return (result);
// 			}
// 			catch (std::string &e)
// 			{
// 				this->set_return_code(400);
// 			}
// 		}
// 	}
// 	set_return_code(400);
// 	Listen result = ConfigUtils::ip_host_parseur("0:0");
// 	return (result);
// }

bool	Request::check_hosts(const std::vector<std::string>& server_names) const
{
	std::vector<std::string> result;

	for (std::map<std::string, std::string>::const_iterator it = this->_header.begin(); it != this->_header.end(); it++)
	{
		if (it->first == "Host")
		{
			std::stringstream		ss(it->second);
			std::string				host;
			while (ss >> host)
				result.push_back(host);
		}
	}
	for (std::vector<std::string>::const_iterator servname = server_names.begin(); servname != server_names.end(); servname++)
	{
		for (std::vector<std::string>::iterator hosts = result.begin(); hosts != result.end(); hosts++)
		{
			if (!(*servname).compare(*hosts))
				return (true);
		}
	}
	return (false);
}

// bool				Request::has_header(std::string& to_find)
// {
// 	if (this->_header.count(to_find))
// 		return (true);
// 	return (false);
// }

const std::string	Request::get_content_type() const
{
	if (this->_header.count("Content-Type"))
		return (this->_header.find("Content-Type")->second);
	return ("");
}

const std::vector<char>	Request::get_body() const
{
	return (this->_body);
}
