
#ifndef MYEXCEPTION_HPP
#define MYEXCEPTION_HPP

#include <iostream>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN  "\033[32m"
#define BLUE  "\033[34m"
#define YELLOW "\033[33m"

class MyException : public std::exception
{
	private:
	// {
		std::string _message;
	// }
	public:
	//{
		// Constructeur qui prend le message personnalisé
		MyException(const std::string &msg);
		MyException(const std::string &msg1, const std::string &msg2);

		virtual ~MyException() throw();
		// what qui renvoie le message personnalisé



		virtual const char* what() const throw();

	// }
};

#endif
