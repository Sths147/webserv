#include "Config.hpp"
#include "MyException.hpp"
#include <fstream>

Config::Config(std::string &nameFile)
{
    std::ifstream sfile(nameFile.c_str()); // Ouvre le file en lecture

    // 1. Vérifier si l'ouverture a réussi
    if (!sfile.is_open()) {
        throw (MyException("Erreur : Impossible d'ouvrir le file ", nameFile));
    }
    std::string tmp;
    while (std::gettmp(sfile, tmp))
    {
        this->_file += tmp + "\n";
    }

    std::cout << this->_file << std::endl;

    if (sfile.bad()) {

        throw(MyException("Erreur critique lors de la lecture du file."));

    } else if (!sfile.eof()) {

        throw(MyException("Une erreur inattendue est survenue avant la fin du file."));

    }
    sfile.close();
}

Config::~Config()
{
}
