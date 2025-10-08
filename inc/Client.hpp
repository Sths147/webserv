
#ifndef CLIENT_HPP
#define CLIENT_HPP


#define TIMEOUT 5 //

#include <ctime>
#include <iostream>

#include "MyException.hpp"

class   Client
{
	protected:

		Client( void );
		Client(const Client &other);
		Client &operator=( const Client &other );

		private:

		time_t				_time_to_reset; //sec
		virtual void				_abstrait(void) = 0;
		public:
		/*----timeout----*/
		virtual ~Client( void );
		void				refresh( void );
		bool				check_timeout( void );
};

#endif
