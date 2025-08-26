/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sithomas <sithomas@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 10:20:10 by sithomas          #+#    #+#             */
/*   Updated: 2025/08/26 16:06:18 by sithomas         ###   ########.fr       */
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
		// Listen										_listen;
		const std::string							_server_name;
		std::vector<char>							_body;
        Request();

    public:
        Request(std::vector<char>&);
        ~Request();
        // Request&    operator=(const Request&);
        const std::string							get_type() const;
        const std::string							get_target() const;
        const std::string							get_http_version() const;
		const std::string							parse_request_type(std::vector<char>& buff);
		const std::string							parse_request_target(std::vector<char>& buff);
		const std::string							parse_http_type(std::vector<char>& buff);
		// Listen										set_listen();
		bool										check_hosts(std::vector<std::string>&) const;
		std::map<std::string, std::string>			parse_header(std::vector<char>& buff);
		const std::string							get_crlf_line(std::vector<char>& buff);
		unsigned short int							get_return_code() const;
		const std::string							parse_key(std::string& line);
		void										set_return_code(const unsigned short int&);
		void										parse_headers();
};
