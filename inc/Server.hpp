

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


		std::vector<Listen>			&get_listen( void );
		std::vector<std::string>	&get_index( void );
		std::vector<int>			&get_error_page( void );
		std::vector<std::string>	&get_server_name( void );
		std::vector<std::string>	&get_allow_methods( void );
		std::string					&get_client_max_body_size( void );
		std::string					&get_root( void );


		~Server();
};

#endif
