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
		if (first_char_pos != std::string::npos && tmp[first_char_pos] != '#') {
				this->_file += tmp + "\n";
		}
		// that the commentary or the empty line
		// else {
		// 	std::cout << tmp << std::endl;
		// }
	}

	// Debug Print all the content stocked
	// std::cout << YELLOW <<"the file.conf :\n" << RESET << this->_file << YELLOW <<"eof" << RESET<< std::endl;

	// 5. Check if we read all the file or not.
	if (sfile.bad()) {

		throw(MyException("Error: Critical error when reading the file."));

	} else if (!sfile.eof()) {

		throw(MyException("Error : The file wasn't totaly readed."));

	}
	sfile.close();
}

static void serverDirectiveParsing(std::string &line) {

	for (size_t i = ConfigUtils::get_pos(); i < line.size(); i++) {
		char c = line[i];

		if (c == ' ' || c == '\t')
			continue;
		if (c == '{') {
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

static std::string locationDirectiveParsing(std::string line, bool &b) {

	std::vector<std::string> vec = ConfigUtils::split(line, ' ');

	if (vec.size() < 2 && (vec[1][0] != '{'|| vec[2][0] != '{'))
		throw (std::string("Error : bad format on this line..."));
	size_t i = 0;
	if (vec[0][0] == '=') {
		if (vec[0].size() > 1)
			throw (std::string("Error : bad format on this line..."));
		b = true;
		i++;
	}
	if (vec[i][0] != '/')
		throw (std::string("Error : bad format on this line..."));
	if (vec.size() > (i + 2) && vec[i + 2][0] != '#')
		throw (std::string("Error : bad format on this line..."));
	return (vec[i]);
}

void			Config::set_in_location( std::string &directive, std::string &line, int &server, int &location, bool &in_location) {

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

	} else if (directive == "autoindex") {

		std::string arg =  ConfigUtils::get_one_token(line);
		if (arg == "on")
			this->_vConfServer[server].set_inlocation_autoindex(location, ON);
		else if (arg == "off")
			this->_vConfServer[server].set_inlocation_autoindex(location, OFF);
		else
			throw (MyException("Error : autoindex unknow value", line));

	} else if (directive == "return") {

		std::vector<std::string> vec_arg =  ConfigUtils::get_multi_token(line);
		if (vec_arg[0] == "301")
			this->_vConfServer[server].set_inlocation_return(location, vec_arg[1]);
		else
			throw (MyException("Error : directive return not allowed", line));

	} else if (directive == "cgi_path") {

		std::string arg = ConfigUtils::get_one_token(line);
		// std::cout << "cgi_path : "<< arg << std::endl;
		try
		{
			this->_vConfServer[server].set_inlocation_cgi_path(location, arg);
		}
		catch(const std::string& e)
		{
			throw (MyException(e, line));
		}
		

	} else if (directive == "cgi_extension") {

		std::string arg = ConfigUtils::get_one_token(line);
		std::cout << "cgi_extension : "<< arg << std::endl;
		this->_vConfServer[server].set_inlocation_cgi_extension(location, arg);

	} else { // here we got "}" or error

		if (directive != "\0")
			throw (MyException("Error : unknown directive...", directive));
		ConfigUtils::check_bracket(line);
		in_location = false;

	}
}

void			Config::set_in_server( std::string &directive, std::string &line, int &server, int &location, bool &in_server) {

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
		try
		{
			this->_vConfServer[server].set_client_max_body_size(arg);
		}
		catch(const std::string &e)
		{
			throw (MyException(e, line));
		}


	} else if (directive == "root") {

		std::string arg =  ConfigUtils::get_one_token(line);
		this->_vConfServer[server].set_root(arg);

	} else if (directive == "autoindex") {

		std::string arg =  ConfigUtils::get_one_token(line);
		if (arg == "on")
			this->_vConfServer[server].set_autoindex(ON);
		else if (arg == "off")
			this->_vConfServer[server].set_autoindex(OFF);
		else
			throw (MyException("Error : autoindex unknow value", line));

	} else if (directive == "return") {

		std::vector<std::string> vec_arg =  ConfigUtils::get_multi_token(line);

		if (vec_arg[0] == "301")
			this->_vConfServer[server].set_return(vec_arg[1]);
		else
			throw (MyException("Error : directive return not allowed", line));

	} else { // here we got "}" or error

		if (directive != "\0")
			throw (MyException("Error : unknown directive...", directive));
		ConfigUtils::check_bracket(line);
		in_server = false;
		location = -1;
	}
}


#include <sstream> // for stringstream
void Config::pars( void )
{

	std::stringstream ss(this->_file);
	std::string line;
	int		server = -1, location = -1;
	bool	in_server = false, in_location = false;


	while (std::getline(ss, line)) {

		std::string directive = ConfigUtils::parse_token(line, 0);
		// if (directive != "\0") // without the close bracket line
		// 	std::cout << "\ndirective == " << directive << "\nin location : "<< in_location << std::endl;


		if (directive == "server") {

			if (in_server)
				throw (MyException("Error : already in a server you can't get a server in a server"));
			server++;
			in_server = true;
			serverDirectiveParsing(line);
			ConfigServer tmp;
			this->_vConfServer.push_back(tmp);

		} else if (in_server && directive == "location") {

			bool b = 0;
			if (in_location)
				throw (MyException("Error : already in a location you can't get a location in a location"));
			location++;
			in_location = true;
			if (line[ConfigUtils::get_pos()] != ' ')
				throw (MyException("Error : bad format on this line...", line));
			try
			{
				std::string perm = locationDirectiveParsing(line.substr(ConfigUtils::get_pos() + 1), b);
				this->_vConfServer[server].set_new_location(perm, b);
			}
			catch(const std::string &str)
			{
				throw (MyException(str, line));
			}

		} else if (in_location) {

			this->set_in_location(directive, line, server, location, in_location);

		} else if (in_server) {

			this->set_in_server(directive, line, server, location, in_server);

		} else {

			throw (MyException("Error : Directive found not in a Server/location...", line));

		}
	}

	// for (int i = 0; i <= server ; i++)
	// {
	// 	std::cout << YELLOW <<"\nPrint all content of the server n" << i << RESET << std::endl;
	// 	this->_vConfServer[i].print_listen();
	// 	// this->_vConfServer[i].print_index();
	// 	// this->_vConfServer[i].print_error_page();
	// 	// this->_vConfServer[i].print_server_name();
	// 	// this->_vConfServer[i].print_allow_methods();
	// 	// this->_vConfServer[i].print_client_max_body_size();
	// 	// this->_vConfServer[i].print_root();
	// 	// this->_vConfServer[i].print_location();
	// }

}
#include <algorithm>

void	Config::check_lunch( void ) {

	// if no listen one default.
	for (size_t i = 0; i < this->_vConfServer.size(); i++)
	{
		std::vector<Listen> vec_listen = this->_vConfServer[i].get_listen();
		if (vec_listen.size() == 0){
			this->_vConfServer[i].set_raw_listen(Listen(0, 8080));
		}
	}

	std::map<unsigned int, std::vector<unsigned int> > map_port_ip;
	for (size_t i = 0; i < this->_vConfServer.size(); i++) {

		std::vector<Listen> vec_listen = this->_vConfServer[i].get_listen();
		for (size_t j = 0; j < vec_listen.size(); j++) {

			unsigned int port = vec_listen[j].port;
			unsigned int ip = vec_listen[j].ip;
			if (map_port_ip.find(port) != map_port_ip.end()) {

				if (ip == 0) {

					this->_vConfServer[i].set_listen_lunch_false(j);

				} else if (std::find(map_port_ip[port].begin(), map_port_ip[port].end(), 0) != map_port_ip[port].end()) {

					this->_vConfServer[i].set_listen_lunch_false(j);

				} else if (std::find(map_port_ip[port].begin(), map_port_ip[port].end(), ip) != map_port_ip[port].end()) {

					this->_vConfServer[i].set_listen_lunch_false(j);

				} else {

					map_port_ip[port].push_back(ip);

				}

			} else {

				map_port_ip[port].push_back(ip);
			}
		}
	}
}




ConfigServer	&Config::copy_config_server( const int &i ) { return (this->_vConfServer[i]); }
size_t	Config::nb_of_server() { return (this->_vConfServer.size()); }

Config::~Config() {}
