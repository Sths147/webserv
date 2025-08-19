


#ifndef CONFIG_HPP
#define CONFIG_HPP


#include "ConfigUtils.hpp"
#include "ConfigServer.hpp"



class Config
{
	private:
		Config(void);

		std::string _file; // containe all the file without empty line commentary

		std::vector<ConfigServer > _vConfServP;


	public:
		Config(std::string &nameFile);
		void parsingFile( void );
		~Config(void);

};

#endif
