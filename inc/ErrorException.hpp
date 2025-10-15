
#ifndef ERROREXCEPTION_HPP
#define ERROREXCEPTION_HPP

#include <iostream>

class ErrorException : public std::exception
{
	private:
		const unsigned short int _code;
	public:
		ErrorException(const unsigned short int &code): _code(code) {};

		virtual ~ErrorException() throw() {};
		// virtual const char* what() const throw();

		//return code
		unsigned short int	get_return() const{ return this->_code;}
};

#endif
