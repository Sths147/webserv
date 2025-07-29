#include "Server.hpp"

int main(int ac, char **av)
{

    if (ac == 2)
        Server webserv(av[1]);
    
}
