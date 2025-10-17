
#ifndef CLIENT_HPP
#define CLIENT_HPP


#define TIMEOUT 5 //

#include <ctime>
#include <iostream>

#include "MyException.hpp"

class	Client
{
	protected:

		Client( void );
		Client(const Client &other);
		Client &operator=( const Client &other );
		bool				_response_created;

	private:
		time_t				_time_to_reset; //sec

	public:
		/*----timeout----*/
		virtual ~Client( void );

		void				refresh( void );
		bool				check_timeout( void );
		bool				check_response_created( void );

		virtual void				del_epoll_and_close( int epoll_fd ) = 0;
};

#endif
