


#ifndef CONFIGLOCATION_HPP
#define CONFIGLOCATION_HPP


#include "ConfigUtils.hpp"


class ConfigLocation
{
	protected:
		ConfigLocation( void );

		std::string _listen;
		std::string _host;
		std::string _server_name;
		std::string _client_max_body_size;
		std::string _root;
		std::string _index;
		std::string _allow_methods;
		std::string _error_page;


	public:
		~ConfigLocation();
};





#endif
