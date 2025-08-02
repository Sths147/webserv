

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"

class Server
{
	private:
		Config _conf;
		Server(void);
	public:
		Server(std::string nameFile);
		~Server();
};

#endif
