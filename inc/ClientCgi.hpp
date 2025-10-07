
#ifndef CLIENTFD_HPP
#define CLIENTFD_HPP


#define TIMEOUT 5 //

#include <ctime>
#include <iostream>

class ClientFd
{
	private:


        int                 _fd_in; // pipe_in[1]
		std::string			_body_request;
        int                 _fd_out; // pipe_out[0]
		std::string			_output_cgi;
        


	public:
		ClientFd( void );
		// ClientFd(const &listen);
		ClientFd &operator=( const ClientFd &other );
		~ClientFd( void );



};

#endif
