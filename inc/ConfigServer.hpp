


#ifndef CONFIGSERVER_HPP
#define CONFIGSERVER_HPP


#include "ConfigUtils.hpp"
#include "ConfigLocation.hpp"


class ConfigServer
{
	private:

		int _listen; // 0 to 65535
		std::string _host;
		std::string _server_name;
		std::string _client_max_body_size;
		std::string _root;
		std::vector<std::string> _index;
		std::vector<std::string> _allow_methods;
		std::vector<std::string> _error_page;

		std::vector<ConfigLocation> _vConfLocaP;

	public:
		ConfigServer( void );
		ConfigServer( const ConfigServer &other );

		/* --- set this server --- */
		void	set_listen( const std::string &str);
		void	set_host( const std::string &str);
		void	set_server_name( const std::string &str);
		void	set_client_max_body_size( const std::string &str);
		void	set_root( const std::string &str);
		void	set_index( const std::string &str);
		void	set_allow_methods( const std::string &str);
		void	set_error_page( const std::string &str);
		void	set_new_location( const std::string &str);

		/* --- set in vector location index--- */
		void	set_inlocation_root(const int &i, const std::string &str);
		void	set_inlocation_index(const int &i, const std::string &str);
		void	set_inlocation_allow_methods(const int &i, const std::string &str);
		void	set_inlocation_error_page(const int &i, const std::string &str);




		~ConfigServer();
};





#endif
