/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sithomas <sithomas@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 14:45:52 by sithomas          #+#    #+#             */
/*   Updated: 2025/09/10 15:01:01 by sithomas         ###   ########.fr       */
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
		std::string											_path;
		std::string											_http_type;
		std::string											_arguments;
		std::string											_reason_phrase;
		std::string											_content_type;
		std::map<std::string, std::string>					_header;
		std::string											_body;
		bool												_autoindex;

	public:
		Response();
		Response(Request &request, Server &server);
		~Response();
		Response&	operator=(const Response&);
		const std::string	determine_final_path(Request& request, Server& server);
		void				set_error_response(Server& server);
		void				set_status(const unsigned short int& code);
		void				write_response(int&	client_fd);
		void				set_get_response();
		void				set_get_headers();
		void				set_error_headers();
		void				set_post_response(Request& request);
		void				set_delete_response(Request& request);
		void				set_delete_headers();
		void				fill_body_with_error_pages(Server& server);
		std::string			set_content_type(const std::string& path);
		const std::string&	get_connection_header() const;
		const unsigned short int&					get_status_code() const;
		const std::string&							get_arguments() const;
		const std::string&							get_reason_phrase() const;
		const std::string&							get_content_type() const;
		const std::string&							get_body() const;
		const std::string&							get_path() const;
		const std::string&							get_http_type() const;
		const std::map<std::string, std::string>&	get_headers() const;
		const bool&									get_autoindex() const;
		void				set_post_headers();
		void				check_allowed_method(const std::string& _method_requested, Server& server);
		void				set_redirect(Server& server);
		void				open_file(std::ofstream& file, std::vector<char>& buff);
};

#endif
