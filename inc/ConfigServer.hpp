


#ifndef CONFIGSERVER_HPP
#define CONFIGSERVER_HPP


#include "ConfigUtils.hpp"
#include "ConfigLocation.hpp"

struct Listen
{
	unsigned int ip;
	unsigned int port;
	Listen(int cip, int cport) : ip(cip), port(cport) {}
};


class ConfigServer
{
	private:

		std::vector<Listen>	_listen; // 65535
		std::vector<std::string>	_server_name;
		std::string					_client_max_body_size;
		std::string					_root;
		std::vector<std::string>	_index;
		std::vector<std::string>	_allow_methods;
		std::vector<std::string>	_error_page;

		std::vector<ConfigLocation>	_vConfLocaP;


		void	print_server_name( void );
		void	print_client_max_body_size( void );
		void	print_root( void );
		void	print_index( void );
		void	print_allow_methods( void );
		void	print_error_page( void );

		public:

		void	print_listen( void );



		ConfigServer( void );
		ConfigServer( const ConfigServer &other );

		/* --- set this server --- */
		void	set_listen( const std::string &str);
		void	set_server_name( const std::vector<std::string> &vec );
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
