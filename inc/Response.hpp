/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sithomas <sithomas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 14:45:52 by sithomas          #+#    #+#             */
/*   Updated: 2025/10/14 14:33:07 by sithomas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include "header.hpp"
#include "Server.hpp"
#include "Request.hpp"
class Client;
#include "ClientCgi.hpp"

class ClientCgi;

class Response
{
	private:
		Server												*_server;
		Request												*_req;
		unsigned short int									_status_code;
		bool												_autoindex;
		std::string											_path;
		std::string											_http_type;
		std::string											_arguments;
		std::string											_reason_phrase;
		std::string											_content_type;
		std::map<std::string, std::string>					_header;
		std::string											_body;
		std::string											_type;

		std::vector<std::string>							_envp;
		std::string											_path_cgi;
		std::string											_script_name;
		bool												_cgi_started;
		Client												*_cgi_get;
		Client												*_cgi_post;
		std::map<int, Client *> 							&_fd_to_info;
		const int											&_epoll_fd;

		Response();
		void						_creat_envp(Request &req);
		std::vector<const char *>	_extrac_envp( void );
		bool						_is_cgi(Request& request, Server& server);
	public:
		Response(Request &request, Server &server, std::map<int, Client *> &fd_to_info, const int &epoll_fd, const int &client_fd, std::vector<Server *> &vec_server);
		~Response();
		Response&	operator=(const Response&);
		const std::string	determine_final_path(Request& request, Server& server);
		void				set_error_response(Server& server);
		void				set_status(const unsigned short int& code);
		std::string			construct_response(void);
		std::string			construct_response_cgi(void);
		void				set_body(const std::string &str);
		void				set_get_response();
		void				set_get_headers();
		void				set_error_headers();
		void				set_cgi_headers();
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
		const bool&									get_cgi_status() const;
		const int&									get_cgi_fd1() const;
		const int&									get_cgi_fd2() const;
		const pid_t&								get_cgi_pid() const;
		void				set_post_headers();
		void				check_allowed_method(const std::string& _method_requested, Server& server);
		void				set_redirect(Server& server);
		void				open_file(std::ofstream& file, std::vector<char>& buff);
		void 				print_headers() const ;
		int					exec_cgi(std::map<int, Client *> &fd_to_info, const int &epoll_fd, const int &client_fd, std::vector<Server *> &vec_server);
		int					cgi(const char *path, const char **script, const char **envp, std::map<int, Client *> &fd_to_info, const int &epoll_fd, const int &client_fd, std::vector<Server *> &vec_server);
		void				null_cgi( void);


	};

#endif
