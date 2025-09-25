/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sithomas <sithomas@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 10:20:10 by sithomas          #+#    #+#             */
/*   Updated: 2025/09/10 14:40:19 by sithomas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

class Request
{
    private:
		unsigned short int							_return_code;
		const std::string							_type;
		const std::string							_target;
		const std::string							_http_type;
		std::map<std::string, std::string>			_header;
		const std::string							_server_name;
		std::vector<char>							_body;
		Request();

	public:
		Request(std::vector<char>&);
		~Request();
		Request&    								operator=(const Request&);
		const std::string							get_type() const;
		const std::string							get_target() const;
		const std::string							get_http_version() const;
		const std::string							parse_request_type(std::vector<char>& buff);
		const std::string							parse_request_target(std::vector<char>& buff);
		const std::string							parse_http_type(std::vector<char>& buff);
		const std::map<std::string, std::string>	get_headers() const;
		// Listen									set_listen();
		bool										check_hosts(const std::vector<std::string>&) const;
		std::map<std::string, std::string>			parse_header(std::vector<char>& buff);
		const std::string							get_crlf_line(std::vector<char>& buff);
		unsigned short int							get_return_code() const;
		const std::string							parse_key(std::string& line);
		void										set_return_code(const unsigned short int&);
		void										parse_headers();
		const std::string							get_content_type() const;
		const std::vector<char>						get_body() const;
		const std::string							get_header(const std::string&) const;
		void 										print_headers() const;
		void										print_body() const;
		void										add_body(std::vector<char> );
};
