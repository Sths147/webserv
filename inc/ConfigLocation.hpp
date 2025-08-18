


#ifndef CONFIGLOCATION_HPP
#define CONFIGLOCATION_HPP


#include "ConfigUtils.hpp"


class ConfigLocation
{
	private:

		ConfigLocation( void );
		std::string _perm; //location "/" {
		std::string _root;
		std::vector<std::string> _index;
		std::vector<std::string> _allow_methods;
		std::vector<std::string> _error_page;

	public:

		ConfigLocation( const std::string &perm );
		ConfigLocation( const ConfigLocation &other );

		void	set_root( const std::string &str);
		void	set_index( const std::string &str);
		void	set_allow_methods( const std::string &str);
		void	set_error_page( const std::string &str);

		~ConfigLocation();
};





#endif
