
#include <iostream>

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
