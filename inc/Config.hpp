


#ifndef CONFIG_HPP
#define CONFIG_HPP


#include "ConfigUtils.hpp"
#include "ConfigServer.hpp"



class Config
{
	private:
		Config(void);

		std::string _file; // containe all the file without empty line commentary

		std::vector<ConfigServer > _vConfServer;
		void	set_in_location( std::string &directive, std::string &line, int &server, int &location, bool &in_location);
		void	set_in_server( std::string &directive, std::string &line, int &server, int &location, bool &in_server);


		public:

		Config(std::string nameFile);
		void			parsingFile( void );
		ConfigServer	&copy_config_server( const int &i );
		size_t			nb_of_server();
		~Config(void);

};

#endif
