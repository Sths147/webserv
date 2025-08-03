#include "Config.hpp"
#include "MyException.hpp"
#define SPACE " \t\n\r\f\v"

#include <fstream>

Config::Config(std::string &nameFile) : _file("")
{

	// 1. Open the config_file to read.
	std::ifstream sfile(nameFile.c_str());

	// 2. Check if is open.
	if (!sfile.is_open()) {
		throw (MyException("Erreur : the file can't be open ", nameFile));
	}

	// 3. Read all the config_file.
	std::string tmp;
	while (std::getline(sfile, tmp))
	{

		// 4. Stock stock all the content maybe usefull. (without empty line and commentary)
		size_t first_char_pos = tmp.find_first_not_of(SPACE);
		if (first_char_pos != std::string::npos && tmp[first_char_pos] != '#'){
				this->_file += tmp + "\n";
		}
		// that the commentary or the empty line
		// else {
		// 	std::cout << tmp << std::endl;
		// }
	}

	// Debug Print all the content stocked
	std::cout << YELLOW <<"the file.conf :\n" << RESET << this->_file << YELLOW <<"eof" << RESET<< std::endl;

	// 5. Check if we read all the file or not.
	if (sfile.bad()) {

		throw(MyException("Error: Critical error when reading the file."));

	} else if (!sfile.eof()) {

		throw(MyException("Error : The file wasn't totaly readed."));

	}
	sfile.close();

}

static void serverDirectiveParsing(std::string &line){
	for (size_t i = ConfigUtils::get_pos(); i < line.size(); i++) {
		char c = line[i];

		if (c == ' ' || c == '\t')
			continue;
		if (c == '{'){
			size_t index;
			for (index = i + 1; index < line.size(); index++)
			{
				c = line[index];
				if (c == ' ' || c == '\t' || c == '\r')
					continue;
				if (c == '#')
					break;
				else
					throw (MyException("Error : bad format on this line...\n", line));

			}
			if (c == '#' || index >= line.size())
				break;
		}
	}
}

static std::string locationDirectiveParsing(std::string &line){
	std::string token;
	for (size_t i = ConfigUtils::get_pos(); i < line.size(); i++) {
		char c = line[i];

		if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ';' || c == '}')
			continue;
		if (c == '/'){
			size_t index;
			token += c;
			for (index = i + 1; index < line.size(); index++)
			{
				c = line[index];
				if (isalpha(c) || c == '/'){
					token += c;
					continue;
				}
				if (c == ' ' &&  index + 1 < line.size() && line[index + 1] == '{') {

					if (ConfigUtils::find_first_not_of_space(line, index + 2) == std::string::npos || line[ConfigUtils::get_pos()] == '#') {
						return (token);
					} else {
						throw (MyException("t Error : bad format on this line...\n", line));
					}
				}
				else
					throw (MyException("e Error : bad format on this line...\n", line));
			}
			if (c == '{' || index >= line.size())
				break;
			i = index;
		}
	}
	return (token);
}

#include <sstream> // pour stringstream
void Config::parsingFile( void )
{

	std::stringstream ss(this->_file);
	std::string line;
	int server = -1, location = -1, in_server = 0;

	while (std::getline(ss, line)) {
		// size_t pos = 0;
		std::string directive = ConfigUtils::parseDirective(line);

		if (directive == "server"){

			// std::cout << "server directive creat a vector server" << std::endl;
			server++;
			in_server = 1;
			serverDirectiveParsing(line);
			this->_vConfServP.push_back(ConfigServer());


		} else if (in_server && directive == "location"){

			// std::cout << "location directive " << directive << std::endl;
			location++;
			std::string perm = locationDirectiveParsing(line);
			// std::cout << perm << std::endl;
			this->_vConfServP[server].set_new_location(perm);

			//do all the pars for location

		} else if (in_server){

			std::cout << "other directive " << directive << std::endl;

		} else {

			throw (MyException("Error : Server directive not found can't save config...\n", line));

		}


	}

}













Config::~Config() {}
