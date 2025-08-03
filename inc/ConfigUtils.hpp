
#ifndef CONFIGUTILS_HPP
#define CONFIGUTILS_HPP


#include <iostream>
#include <vector>

class ConfigUtils
{
	private:
		static size_t _pos;
		ConfigUtils(/* args */);
		~ConfigUtils();

	public:

		static	bool check_after_bracket(std::string str, size_t pos);


		static	size_t		find(std::string &str, std::string strfind);
		static	size_t		find_first_not_of_space(const std::string &str);
		static	size_t		find_first_not_of_space(const std::string &str, size_t pos);
		static	size_t		get_pos( void );
		static	std::string	parseDirective(const std::string& input);

};

#endif
