
#ifndef CLIENT_HPP
#define CLIENT_HPP


#define TIMEOUT 10 //

#include <ctime>
#include <iostream>

#include "MyException.hpp"

class	Client
{
	protected:

		Client( void );
		Client(const Client &other);
		Client &operator=( const Client &other );

		private:

		time_t				_time_to_reset; //sec

		public:
		/*----timeout----*/
		virtual ~Client( void );

		void				refresh( void );
		bool				check_timeout( void );

		virtual void				del_epoll_and_close( int epoll_fd ) = 0;
	};

#endif
