

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"
class Server;

class Server
{
	private:
		Server(void);
		ConfigServer _ConfServer;
		std::vector<int> vector_socket_fd;
	public:
		Server(ConfigServer &config, int epoll_fd);

		const	std::vector<int>							&get_socket_fd( void ) const ;
		bool												check_listen( Listen &tmp ) const;
		const	std::vector<Listen>							&get_listen( void ) const ;
		const	std::vector<std::string>					&get_index( void ) const ;
		const	std::map<unsigned short int, std::string>	&get_error_page( void ) const ;
		const	std::vector<std::string>					&get_server_name( void ) const ;
		const	std::vector<std::string>					&get_allow_methods( void ) const ;
		const	size_t										&get_client_max_body_size( void ) const ;
		const	std::string									&get_root( void ) const ;
		const	autoindexvalue								&get_autoindex( void ) const ;
		const	std::string									&get_return( void ) const ;

		bool												check_location( const std::string key ) ;
		const	std::string									&get_inlocation_location( void ) const ;
		const	std::vector<std::string>					&get_inlocation_index( void ) const ;
		const	std::map<unsigned short int, std::string>	&get_inlocation_error_page( void ) const ;
		const	std::vector<std::string>					&get_inlocation_allow_methods( void ) const ;
		const	std::string									&get_inlocation_root( void ) const ;
		const	autoindexvalue								&get_inlocation_autoindex( void ) const ;
		const	std::string									&get_inlocation_return( void ) const ;
		const	std::string									&get_inlocation_cgi_path( void ) const ;
		const	std::string									&get_inlocation_cgi_extension( void ) const ;

		~Server();
};

#endif
