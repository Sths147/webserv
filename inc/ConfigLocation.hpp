


#ifndef CONFIGLOCATION_HPP
#define CONFIGLOCATION_HPP


#include "ConfigUtils.hpp"


class ConfigLocation
{
	private:

		ConfigLocation( void );
		std::string					_perm; // location "/" {
		std::vector<std::string>	_index;
		std::vector<int>			_error_page;
		std::vector<std::string>	_allow_methods;
		std::string					_root;

		void	print_index( void );
		void	print_error_page( void );
		void	print_allow_methods( void );
		void	print_root( void );

	public:

		void	print_all( void );



		ConfigLocation( const std::string &perm );

		void	set_index( const std::vector<std::string> &str);
		void	set_error_page( const std::vector<std::string> &str);
		void	set_allow_methods( const std::vector<std::string> &str);
		void	set_root( const std::string &str);



		bool	check_perm( const std::string &key ) const ;
		const	std::vector<std::string>	&get_index( void ) const ;
		const	std::vector<int>			&get_error_page( void ) const ;
		const	std::vector<std::string>	&get_allow_methods( void ) const ;
		const	std::string					&get_root( void ) const ;

		~ConfigLocation();
};





#endif
