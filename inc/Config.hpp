
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "AConfigParsing.hpp"

class Config : public AConfigParsing
{
    private:
        Config(void);
        
        std::string _file;
        
    public:
        Config(std::string &nameFile);
        ~Config(void);
    
};

#endif
