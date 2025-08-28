/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sithomas <sithomas@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 14:45:52 by sithomas          #+#    #+#             */
/*   Updated: 2025/08/28 13:34:24 by sithomas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "header.hpp"
#include "Server.hpp"
#include "Request.hpp"


class Response
{
    private:
		unsigned short int									_status_code;
		const std::string									_path;
		const std::string									_http_type;
		bool												_isdir;
		std::string											_arguments;
		std::string											_reason_phrase;
		std::string											_content_type;
		std::map<std::string, std::string>					_header;
		std::string											_body;

	public:
		Response(Request& request, Server& server);
		~Response();

		const std::string	determine_final_path(Request& request, Server& server);
		void				set_error_response(Server& server);
		void				set_status(const unsigned short int& code);
		void				write_response(int&	client_fd);
		void				set_get_response();
		void				set_get_headers();
		void				set_error_headers();
		void				fill_body_with_error_pages(Server& server);
		std::string			set_content_type(const std::string& path);
};

#endif
