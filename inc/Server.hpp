

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"

class Server
{
	private:
		Server(void);
		ConfigServer _ConfServer;
	public:
		Server(ConfigServer &Config);


		const	std::vector<Listen>			&get_listen( void ) const ;
		const	std::vector<std::string>	&get_index( void ) const ;
		const	std::vector<int>			&get_error_page( void ) const ;
		const	std::vector<std::string>	&get_server_name( void ) const ;
		const	std::vector<std::string>	&get_allow_methods( void ) const ;
		const	std::string					&get_client_max_body_size( void ) const ;
		const	std::string					&get_root( void ) const ;


		bool								check_perm( const std::string key ) ;
		const	std::vector<std::string>	&get_inlocation_index( void ) const ;
		const	std::vector<int>			&get_inlocation_error_page( void ) const ;
		const	std::vector<std::string>	&get_inlocation_allow_methods( void ) const ;
		const	std::string					&get_inlocation_root( void ) const ;

		~Server();
};

#endif
