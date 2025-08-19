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
	bool	in_server = false, in_location = false; // just to know if we are on a section server on a location

	while (std::getline(ss, line)) {

		std::string directive = ConfigUtils::parse_token(line, 0);
		// if (directive != "\0") // without the close bracket line
		// 	std::cout << "\ndirective == " << directive << "\nin location : "<< in_location << std::endl;

		if (directive == "server"){

			if (in_server)
				throw (MyException("Error : already in a server we cant get a server in a server"));
			server++;
			in_server = true;
			serverDirectiveParsing(line);
			ConfigServer tmp;
			this->_vConfServP.push_back(tmp);
			// std::cout << "server open"<< server << std::endl;


		} else if (in_server && directive == "location"){

			if (in_location)
				throw (MyException("Error : already in a location we cant get a location in a location"));

			location++;
			in_location = true;
			std::string perm = locationDirectiveParsing(line);
			this->_vConfServP[server].set_new_location(perm);
			// std::cout << perm << std::endl;
			// std::cout << "location open"<< location << std::endl;





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
				in_location = false;
				// here we got "}" or error
				// std::cout << "location close"<< std::endl;
			}







		} else if (in_server) {



				// std::cout << "directive in_server : " << directive << std::endl;

			if (directive == "listen") {

				std::string arg = ConfigUtils::get_one_token(line);
				try
				{
					this->_vConfServP[server].set_listen(arg);
				}
				catch(const std::string& str)
				{
					throw (MyException(str, line));
				}
				// std::cout << "arg listen : " <<  arg << " reste : " << line[ConfigUtils::get_pos()] << std::endl;

			} else if (directive == "index") { // maybe more than one

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				this->_vConfServP[server].set_index(arg);
				// for (size_t i = 0; i < arg.size(); i++)
				// {std::cout << "'" << arg[i] << "'" << "\n";}

			} else if (directive == "error_page") { // maybe more than one

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				this->_vConfServP[server].set_error_page(arg);
				// for (size_t i = 0; i < arg.size(); i++)
				// {std::cout << "'" << arg[i] << "'" << "\n";}

			} else if (directive == "server_name") {

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				this->_vConfServP[server].set_server_name(arg);
				// for (size_t i = 0; i < arg.size(); i++)
				// {std::cout << "'" << arg[i] << "'" << "\n";}

			} else if (directive == "allow_methods") { // maybe more than one

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				this->_vConfServP[server].set_allow_methods(arg);
				// for (size_t i = 0; i < arg.size(); i++)
				// {std::cout << "'" << arg[i] << "'" << "\n";}

			} else if (directive == "client_max_body_size") {

				std::string arg =  ConfigUtils::get_one_token(line);
				this->_vConfServP[server].set_client_max_body_size(arg);//todo convert in bytes

			} else if (directive == "root") {

				std::string arg =  ConfigUtils::get_one_token(line);
				this->_vConfServP[server].set_root(arg);

			} else {

				if (directive != "\0")
					throw (MyException("Error : unknown directive...", directive));
				ConfigUtils::check_bracket(line);
				in_server = false;
				// here we got "}" or error
				// std::cout << "server close"<< std::endl;
			}

		} else {

			throw (MyException("Error : Directive found not in a Server/location...", line));

		}

	}


	for (int i = 0; i <= server ; i++)
	{
		std::cout << YELLOW <<"\nPrint all content of the server n" << i << RESET << std::endl;
		this->_vConfServP[i].print_listen();
		this->_vConfServP[i].print_index();
		this->_vConfServP[i].print_error_page();
		this->_vConfServP[i].print_server_name();
		this->_vConfServP[i].print_allow_methods();
		this->_vConfServP[i].print_client_max_body_size();
		this->_vConfServP[i].print_root();
	}



}













Config::~Config() {}
