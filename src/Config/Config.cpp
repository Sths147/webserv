#include "Config.hpp"
#include "MyException.hpp"
#define SPACE " \t\n\r\f\v"

#include <fstream>


Config::Config( std::string nameFile )
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
			this->_vConfServer.push_back(tmp);

		} else if (in_server && directive == "location"){

			if (in_location)
				throw (MyException("Error : already in a location we cant get a location in a location"));
			location++;
			in_location = true;
			std::string perm = locationDirectiveParsing(line);
			this->_vConfServer[server].set_new_location(perm);

		} else if (in_location) {

			if (directive == "index") {

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				this->_vConfServer[server].set_inlocation_index(location, arg);

			} else if (directive == "error_page") {

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				this->_vConfServer[server].set_inlocation_error_page(location, arg);

			} else if (directive == "allow_methods") {

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				this->_vConfServer[server].set_inlocation_allow_methods(location, arg);

			} else if (directive == "root") {

				std::string arg =  ConfigUtils::get_one_token(line);
				this->_vConfServer[server].set_inlocation_root(location, arg);

			} else { // here we got "}" or error

				if (directive != "\0")
					throw (MyException("Error : unknown directive...", directive));
				ConfigUtils::check_bracket(line);
				in_location = false;

			}

		} else if (in_server) {

			if (directive == "listen") {

				std::string arg = ConfigUtils::get_one_token(line);
				try
				{
					this->_vConfServer[server].set_listen(arg);
				}
				catch(const std::string& str)
				{
					throw (MyException(str, line));
				}

			} else if (directive == "index") {

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				this->_vConfServer[server].set_index(arg);

			} else if (directive == "error_page") {

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				try
				{
					this->_vConfServer[server].set_error_page(arg);
				}
				catch(const std::string& str)
				{
					throw (MyException(str, line));
				}

			} else if (directive == "server_name") {

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				this->_vConfServer[server].set_server_name(arg);

			} else if (directive == "allow_methods") {

				std::vector<std::string> arg =  ConfigUtils::get_multi_token(line);
				this->_vConfServer[server].set_allow_methods(arg);

			} else if (directive == "client_max_body_size") {

				std::string arg =  ConfigUtils::get_one_token(line);
				this->_vConfServer[server].set_client_max_body_size(arg);//todo convert in bytes

			} else if (directive == "root") {

				std::string arg =  ConfigUtils::get_one_token(line);
				this->_vConfServer[server].set_root(arg);

			} else { // here we got "}" or error

				if (directive != "\0")
					throw (MyException("Error : unknown directive...", directive));
				ConfigUtils::check_bracket(line);
				in_server = false;
				location = -1;
			}

		} else {

			throw (MyException("Error : Directive found not in a Server/location...", line));

		}
	}

	for (int i = 0; i <= server ; i++)
	{
		std::cout << YELLOW <<"\nPrint all content of the server n" << i << RESET << std::endl;
		this->_vConfServer[i].print_listen();
		this->_vConfServer[i].print_index();
		this->_vConfServer[i].print_error_page();
		this->_vConfServer[i].print_server_name();
		this->_vConfServer[i].print_allow_methods();
		this->_vConfServer[i].print_client_max_body_size();
		this->_vConfServer[i].print_root();
		this->_vConfServer[i].print_location();
	}

}




ConfigServer	&Config::copy_config_server( const int &i ){ return (this->_vConfServer[i]); }
size_t	Config::nb_of_server(){ return (this->_vConfServer.size()); }

Config::~Config() {}
