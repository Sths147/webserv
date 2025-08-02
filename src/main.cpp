#include "Server.hpp"

int main(int ac, char **av)
{

	if (ac == 2){

		try	{
			Server webserv(av[1]);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}

	}

}
