

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"
class Server;
typedef std::map<unsigned int, std::map<unsigned int, std::vector<Server *> > > map_uint_maps_uint_vec_server;
class Server
{
	private:
		Server(void);
		ConfigServer _ConfServer;
		std::vector<int> vector_socket_fd;
	public:
		Server(ConfigServer &config, int epoll_fd, map_uint_maps_uint_vec_server &map_ip_port_vec_ptrserver);

		const	std::vector<int>							&get_socket_fd( void ) const ;
		const	std::vector<Listen>							&get_listen( void ) const ;
		const	std::vector<std::string>					&get_index( void ) const ;
		const	std::map<unsigned short int, std::string>	&get_error_page( void ) const ;
		const	std::vector<std::string>					&get_server_name( void ) const ;
		const	std::vector<std::string>					&get_allow_methods( void ) const ;
		const	std::string									&get_client_max_body_size( void ) const ;
		const	std::string									&get_root( void ) const ;


		bool												check_location( const std::string key ) ;
		const	std::string									&get_inlocation_location( void ) const ;
		const	std::vector<std::string>					&get_inlocation_index( void ) const ;
		const	std::map<unsigned short int, std::string>	&get_inlocation_error_page( void ) const ;
		const	std::vector<std::string>					&get_inlocation_allow_methods( void ) const ;
		const	std::string									&get_inlocation_root( void ) const ;

		~Server();
};

#endif
