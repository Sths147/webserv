


#ifndef CONFIGSERVER_HPP
#define CONFIGSERVER_HPP


#include "ConfigUtils.hpp"
#include "ConfigLocation.hpp"


class ConfigServer
{
	protected:

		ConfigServer( void );
		std::string _listen;
		std::string _host;
		std::string _server_name;
		std::string _client_max_body_size;
		std::string _root;
		std::string _index;
		std::string _allow_methods;
		std::string _error_page;

		std::vector<ConfigLocation> _vConfLocaP;

	public:
		~ConfigServer();
};





#endif
