
#ifndef CONFIGUTILS_HPP
#define CONFIGUTILS_HPP


#include <iostream>
#include <vector>

class ConfigUtils
{
	private:

		static size_t _pos;
		ConfigUtils(/* args */);

	public:

		~ConfigUtils();
		static	size_t						get_pos( void );

		static	void						check_bracket(const std::string &str);
		static	void						check_after_bracket_semicolon(const std::string &str, size_t pos);
		static	bool						error_page_valid(const int &page);

		static	size_t						find_first_not_of_space(const std::string &str);
		static	size_t						find_first_not_of_space(const std::string &str, size_t pos);

		static	std::string					parse_token(const std::string& input, const size_t pos);
		static	std::vector<std::string>	parse_multi_token(const std::string& input, const size_t pos);

		static	std::string					get_one_token(const std::string &str);
		static	std::vector<std::string>	get_multi_token(const std::string &str);

		static std::vector<std::string> 	split(const std::string &str, char delim);




};

#endif
