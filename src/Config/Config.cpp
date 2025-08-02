#include "Config.hpp"
#include "MyException.hpp"

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
		size_t first_char_pos = tmp.find_first_not_of(" \t\n\r\f\v");
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

#include <sstream> // Nécessaire pour stringstream
void Config::parsingFile( void )
{

	std::stringstream ss(this->_file);
	std::string line("");
	int	bracket = 0, location_block = 0;

	while (std::getline(ss, line)) {

		if (ConfigUtils::find(line, "server {") != std::string::npos) {
			bracket++;
		} else if (location_block == 0 && bracket > 0 && ConfigUtils::find(line, "listen") != std::string::npos) {

			// this->_file.pos

		} else if (location_block == 0 && bracket > 0 && ConfigUtils::find(line, "host") != std::string::npos) {

		} else if (location_block == 0 && bracket > 0 && ConfigUtils::find(line, "server_name") != std::string::npos) {

		} else if (location_block == 0 && bracket > 0 && ConfigUtils::find(line, "client_max_body_size") != std::string::npos) {

		} else if (location_block == 0 && bracket > 0 && ConfigUtils::find(line, "root") != std::string::npos) {

		} else if (location_block == 0 && bracket > 0 && ConfigUtils::find(line, "index") != std::string::npos) {

		} else if (location_block == 0 && bracket > 0 && ConfigUtils::find(line, "allow_methods") != std::string::npos) {

		} else if (location_block == 0 && bracket > 0 && ConfigUtils::find(line, "error_page") != std::string::npos) {

		} else if (location_block == 0 && bracket > 0 && ConfigUtils::find(line, "location") != std::string::npos) {

			ConfigUtils::get_pos();
			location_block = 1;

		} else if (ConfigUtils::find(line, "}") != std::string::npos) {
			if (location_block == 1)
			ConfigUtils::get_pos();
		}
	}

}













Config::~Config() {}
