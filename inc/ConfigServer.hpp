


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

		std::vector<Listen>			_listen;
		std::vector<std::string>	_index;
		std::vector<int>			_error_page;
		std::vector<std::string>	_server_name;
		std::vector<std::string>	_allow_methods;
		std::string					_client_max_body_size; // convert max ?
		std::string					_root;

		std::vector<ConfigLocation>	_vConfLocal;


		public:

		void	print_listen( void );
		void	print_index( void );
		void	print_error_page( void );
		void	print_server_name( void );
		void	print_allow_methods( void );
		void	print_client_max_body_size( void );
		void	print_root( void );
		void	print_location( void );



		std::vector<Listen>			&get_listen( void );
		std::vector<std::string>	&get_index( void );
		std::vector<int>			&get_error_page( void );
		std::vector<std::string>	&get_server_name( void );
		std::vector<std::string>	&get_allow_methods( void );
		std::string					&get_client_max_body_size( void );
		std::string					&get_root( void );

		/* --- set this server --- */
		void	set_listen( const std::string &str);
		void	set_index( const std::vector<std::string> vec );
		void	set_error_page( const std::vector<std::string> vec );
		void	set_server_name( const std::vector<std::string> vec );
		void	set_allow_methods( const std::vector<std::string> vec );
		void	set_client_max_body_size( const std::string &str);
		void	set_root( const std::string &str);
		void	set_new_location( const std::string &perm);

		/* --- set in vector location index--- */
		void	set_inlocation_index(const int &i, const std::vector<std::string> &arg);
		void	set_inlocation_allow_methods(const int &i, const std::vector<std::string> &arg);
		void	set_inlocation_error_page(const int &i, const std::vector<std::string> &arg);
		void	set_inlocation_root(const int &i, const std::string &str);




		ConfigServer( void );
		~ConfigServer();
};





#endif
