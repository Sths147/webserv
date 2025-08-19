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
					throw (MyException("Error : bad format on this line...", line));

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
						throw (MyException("Error : bad format on this line...", line));
					}
				}
				else
					throw (MyException("Error : bad format on this line...", line));
			}
			if (c == '{' || index >= line.size())
				break;
			i = index;
		}
	}
	return (token);

}

#include <sstream> // for stringstream
void Config::parsingFile( void )
{

	std::stringstream ss(this->_file);
	std::string line;
	int		server = -1, location = -1;
	bool	in_server = 0, in_location = 0;

	while (std::getline(ss, line)) {

		std::string directive = ConfigUtils::parse_token(line, 0);
		// if (directive != "\0") // without the close bracket line
		// 	std::cout << "\ndirective == " << directive << "\nin location : "<< in_location << std::endl;

		if (directive == "server"){

			// std::cout << "server directive creat a vector server" << std::endl;
			server++;
			in_server = 1;
			serverDirectiveParsing(line);
			this->_vConfServP.push_back(ConfigServer());

		} else if (!in_location && in_server && directive == "location"){

			// std::cout << "location directive " << directive << std::endl;
			location++;
			in_location = 1;
			std::string perm = locationDirectiveParsing(line);
			this->_vConfServP[server].set_new_location(perm);
			// std::cout << perm << std::endl;





		} else if (in_location) {



				// std::cout << "directive in_location : " << directive << std::endl;

			if (directive == "root") {

				std::string arg =  ConfigUtils::get_one_token(line);
				// std::cout << arg << "\n";

			} else if (directive == "index") { // maybe more than one

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				// for (size_t i = 0; i < arg.size(); i++)
				// {std::cout << "'" << arg[i] << "'" << "\n";}

			} else if (directive == "allow_methods") { // maybe more than one

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				// for (size_t i = 0; i < arg.size(); i++)
				// {std::cout << "'" << arg[i] << "'" << "\n";}

			} else if (directive == "error_page") { // maybe more than one

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				// for (size_t i = 0; i < arg.size(); i++)
				// {std::cout << "'" << arg[i] << "'" << "\n";}

			} else {
				if (directive != "\0")
					throw (MyException("Error : unknown directive...", directive));
				ConfigUtils::check_bracket(line);
				in_location = 0;
				// here we got "}" or error
				// std::cout << "other directive " << directive << " line : '" << line << "'"<< std::endl;
			}







		} else if (in_server) {



				// std::cout << "directive in_server : " << directive << std::endl;

			if (directive == "listen") {

				std::string arg = ConfigUtils::get_one_token(line);
				this->_vConfServP[server].set_listen(arg); //todo stocker

				// std::cout << "arg listen : " <<  arg << " reste : " << line[ConfigUtils::get_pos()] << std::endl;

			} else if (directive == "server_name") {

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				// for (size_t i = 0; i < arg.size(); i++)
				// {std::cout << "'" << arg[i] << "'" << "\n";}
				this->_vConfServP[server].set_server_name(arg);

			} else if (directive == "client_max_body_size") {

				std::string arg =  ConfigUtils::get_one_token(line);
				this->_vConfServP[server].set_client_max_body_size(arg);//todo convert in bytes

			} else if (directive == "root") {

				std::string arg =  ConfigUtils::get_one_token(line);
				this->_vConfServP[server].set_root(arg);//todo check if its good path ?

			} else if (directive == "index") { // maybe more than one


				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				// for (size_t i = 0; i < arg.size(); i++)
				// {std::cout << "'" << arg[i] << "'" << "\n";}

				// this->_vConfServP[server].set_root(arg);//todo check if its good path ?

			} else if (directive == "allow_methods") { // maybe more than one


				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				// for (size_t i = 0; i < arg.size(); i++)
				// {std::cout << "'" << arg[i] << "'" << "\n";}

			} else if (directive == "error_page") { // maybe more than one


				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				// for (size_t i = 0; i < arg.size(); i++)
				// {std::cout << "'" << arg[i] << "'" << "\n";}

			} else {

				if (directive != "\0")
					throw (MyException("Error : unknown directive...", directive));
				ConfigUtils::check_bracket(line);
				in_server = 0;
				// here we got "}" or error
				// std::cout << "other directive " << directive << " line : '" << line << "'"<< std::endl; //SUPP
			}

		} else {

			throw (MyException("Error : Directive found not in a Server/location...", line));

		}

	}


	// for (int i = 0; i <= server; i++)
	// {
	// 	std::cout << YELLOW <<"Print all content of the server n" << i << RESET << std::endl;
	// 	this->_vConfServP[0].print_listen();
	// 	this->_vConfServP[0].print_server_name();
	// 	this->_vConfServP[0].print_client_max_body_size();
	// 	this->_vConfServP[0].print_root();
	// 	this->_vConfServP[0].print_index();
	// 	this->_vConfServP[0].print_allow_methods();
	// 	this->_vConfServP[0].print_error_page();
	// }



}













Config::~Config() {}
