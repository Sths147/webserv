/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sithomas <sithomas@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:22:58 by sithomas          #+#    #+#             */
/*   Updated: 2025/08/06 12:25:14 by sithomas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Request.hpp"

static std::string	get_request_type(std::vector<char>& buff);
static std::string	get_request_target(std::vector<char>& buff);
static std::string	get_http_type(std::vector<char>& buff);
static std::map<std::string, std::string>	parse_header(std::vector<char>& buff);
static std::string							get_crlf_line(std::vector<char>& buff);
static std::string							parse_key(std::string& line);

enum http_types {
    CR = '\r',
    LF = '\n',
    SP = ' ',
	HTAB = '\t'
} t_http_types; 

Request::Request()
: _type("NOTYPE")
{
}

Request::Request(std::vector<char>&buff)
: _return_code(200) , _finished(0) ,_type(get_request_type(buff)), _target(get_request_target(buff)), _http_type(get_http_type(buff)), _header(parse_header(buff)), _body(buff)
{
	(void)_return_code;
	(void)_finished;
}

Request::~Request()
{
}

std::string	Request::get_type()
{
	return (this->_type);
}

std::string	Request::get_target()
{
	return (this->_target);
}

std::string	Request::get_http_version()
{
	return (this->_http_type);
}

/*
A server that receives a method longer than any that it implements SHOULD respond with a 501 (Not Implemented) status code. A server that receives a request-target longer than any URI it wishes to parse MUST respond with a 414 (URI Too Long) status code (see Section 15.5.15 of [HTTP]).
*/

static std::string	get_request_target(std::vector<char>& buff)
{
	std::string					result;
	std::vector<char>::iterator	k = buff.begin();
	if (*k != SP)
		throw std::runtime_error("request must be followed by SP");
	buff.erase(k);
	while (k != buff.end())
	{
		if (*k == SP)
			break;
		else if (*k < 33 || *k > 126)
			std::runtime_error("target must be formed only with printables characters");
		else
		{
			result += *k;
			buff.erase(k);
		}
	}
	if (result.empty() || k == buff.end())
		throw std::runtime_error("check target section for extra SP or no SP after target");
	return (result);
}

static std::string	get_http_type(std::vector<char>& buff)
{
	std::string 				result;
	std::string					to_compare = "HTTP/1.1";
	std::vector<char>::iterator	k = buff.begin();
	
	if (*k != SP)
		throw std::runtime_error("parsing error: check http version");
	buff.erase(k);
	for (int i = 0; i < 8; i++)
	{
		if (k == buff.end())
			throw std::runtime_error("parsing error: check http version");
		if (i == 5 || i == 7)
		{
			if (!isdigit(*k))
				throw std::runtime_error("parsing error: check http version");
		}
		else if (*k != to_compare[i])
		{
			throw std::runtime_error("parsing error: check http version");
		}
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
	throw std::runtime_error("parsing error : request line must end with LF or CRLF");
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

static std::string	get_request_type(std::vector<char>& buff)
{
	std::string result;
	std::vector<char>::iterator k = buff.begin();
	skip_crlf(buff);
	for (int i = 0; i < 10; i++)
	{
		if (i == 9 || k == buff.end())
			throw std::runtime_error(" in request type");
		else if (*k == SP)
		{
			break;	
		}
		else if (*k >= 'A' && *k <= 'Z')
		{
			result += *k;
			buff.erase(k);
		}
		else
			throw std::runtime_error(" in request type");	
	}
	if (result.empty() || (!result.compare("GET") && !result.compare("POST") && !result.compare("DELETE")))
		throw std::runtime_error(" in request type");
	return (result);
}

static std::map<std::string, std::string>	parse_header(std::vector<char>& buff)
{
	std::map<std::string, std::string>	result;
	std::string							line = get_crlf_line(buff);
	std::string							key;
	while (line.length() != 0)
	{
		key = parse_key(line);
		if (!result[key].empty())
			throw std::runtime_error("parse error : double definition of same header");
		const std::string OWS = " \t";
		if (line.find_first_not_of(OWS) >= line.find_last_not_of(OWS))
			throw std::runtime_error("error in value");
		line = line.substr(line.find_first_not_of(OWS), line.find_last_not_of(OWS) + 1);
		if (line.length() == 0)
			throw std::runtime_error("error in value");
		result[key] = line;
		// std::cout << key << ": " << line << std::endl;
		line = get_crlf_line(buff);
	}
	return (result);
}

static std::string							get_crlf_line(std::vector<char>& buff)
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
			throw std::runtime_error("parse error: unauthorized character in header");
		return_value += *k;
		buff.erase(k);
	}
	throw std::runtime_error("parse error in request's header field");
	return (NULL);
}

static std::string							parse_key(std::string& line)
{
	std::string	return_value;
	size_t 		n = line.find(':');
	
	if (n == std::string::npos)
		throw std::runtime_error("header parse error: no ':' found in a line");
	if (n == 0)
		throw std::runtime_error("header parse error: empty key");
	if (n == line.length() - 1)
		throw std::runtime_error("header parse error: empty value");
	return_value = line.substr(0, n);
	line = line.substr(n);
	return (return_value);
}