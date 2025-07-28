/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sithomas <sithomas@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:27:39 by sithomas          #+#    #+#             */
/*   Updated: 2025/07/28 18:20:15 by sithomas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

std::vector<Server> parse(std::ifstream& file);

int main(int ac, char** av)
{
	try {
		if (ac != 2)
			throw std::runtime_error("one and only one configuration file required as argument");
		if (!av[1] || av[1][0] == '\0')
			throw std::runtime_error("config file name is empty");
		std::ifstream conf_file(av[1]);
		if (!conf_file)
			throw std::runtime_error("wrong");
		parse(conf_file);
	} catch (std::exception& e)
	{
		std::cout << "Error : " << e.what() << std::endl;
	}
}

std::vector<Server> parse(std::ifstream& conf_file)
{
	std::string jjj;
	std::vector<Server> result;
	
	while (!conf_file.eof())
	{
		std::getline(conf_file, jjj);
		if (jjj.find("server {") != std::string::npos)
		{
			//start server BRUH
		}
	}
	if (result.size() == 0)
		throw std::runtime_error("no server location in config file");
	return (result);
}