


#ifndef CONFIGSERVER_HPP
#define CONFIGSERVER_HPP


#include "ConfigUtils.hpp"
#include "ConfigLocation.hpp"


class ConfigServer
{
	private:

		std::vector<Listen>								_listen;
		std::vector<std::string>						_index;
		std::map<unsigned short int, std::string>		_error_page;
		std::vector<std::string>						_server_name;
		std::vector<std::string>						_allow_methods;
		size_t											_client_max_body_size;
		std::string										_root;
		autoindexvalue									_autoindex;

		int												_last_i_location;
		std::vector<ConfigLocation>						_vConfLocal;


		public:

			void	print_listen( void );
			void	print_index( void );
			void	print_error_page( void );
			void	print_server_name( void );
			void	print_allow_methods( void );
			void	print_client_max_body_size( void );
			void	print_root( void );
			void	print_location( void );


			/* --- set this server --- */
			void	set_listen( const std::string &str);
			void	set_raw_listen(Listen lis);
			void	set_listen_lunch_false( size_t index);
			void	set_index( const std::vector<std::string> vec );
			void	set_error_page( const std::vector<std::string> vec );
			void	set_server_name( const std::vector<std::string> vec );
			void	set_allow_methods( const std::vector<std::string> vec );
			void	set_client_max_body_size( const std::string &str);
			void	set_root( const std::string &str);
			void	set_autoindex( const autoindexvalue v );
			void	set_new_location( const std::string &perm, const bool &b);

			/* --- set in vector location index--- */
			void	set_inlocation_index(const int &i, const std::vector<std::string> &arg);
			void	set_inlocation_allow_methods(const int &i, const std::vector<std::string> &arg);
			void	set_inlocation_error_page(const int &i, const std::vector<std::string> &arg);
			void	set_inlocation_root(const int &i, const std::string &str);
			void	set_inlocation_autoindex(const int &i, const autoindexvalue b );



			/* --- GET--- */
			const	std::vector<Listen>									&get_listen( void ) const ;
			const	std::vector<std::string>							&get_index( void ) const ;
			const	std::map<unsigned short int, std::string>			&get_error_page( void ) const ;
			const	std::vector<std::string>							&get_server_name( void ) const ;
			const	std::vector<std::string>							&get_allow_methods( void ) const ;
			const	size_t												&get_client_max_body_size( void ) const ;
			const	std::string											&get_root( void ) const ;
			const	autoindexvalue										&get_autoindex( void ) const ;

			/* --- GET inlocation --- */
			bool														check_location( std::string key ) ;
			const	std::string											&get_inlocation_location( void ) const ;
			const	std::vector<std::string>							&get_inlocation_index( void ) const ;
			const	std::map<unsigned short int, std::string>			&get_inlocation_error_page( void ) const ;
			const	std::vector<std::string>							&get_inlocation_allow_methods( void ) const ;
			const	std::string											&get_inlocation_root( void ) const ;
			const	autoindexvalue										&get_inlocation_autoindex( void ) const ;


			ConfigServer( void );
			~ConfigServer();
};





#endif
