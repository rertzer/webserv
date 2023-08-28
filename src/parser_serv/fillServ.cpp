/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fillServ.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 09:44:50 by pjay              #+#    #+#             */
/*   Updated: 2023/08/28 11:12:22 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void printVec(std::vector<std::string> vec)
{
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
		std::cout << *it << std::endl;
}

int fillServ(std::string av, std::vector<Server>& serv)
{
	std::ifstream conf;
	std::string lineString;
	std::vector<bool> bracketOpen;
	std::vector<std::string> servStrings;
	bool servOpen = false;
	int countLine = 0;
	int i = 0;
	try
	{
		conf.open(av.c_str(), std::fstream::in);
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		return (-1);
	}
	while (getline(conf, lineString))
	{
		countLine++;
		if (lineString.find("#") != std::string::npos)
			lineString = lineString.substr(0, lineString.find("#"));
		if (lineString.find_first_not_of(" \t") == std::string::npos)
			continue;
		if (lineString.find("server {") != std::string::npos) // I want it to skip the server line
		{
			//std::cout << "enter here" << std::endl;
			servOpen = true;
			bracketOpen.push_back(true);
			//std::cout << "countLine = " << countLine << std::endl;
			continue;
		}
		if (servOpen == true)
		{
			//std::cout << "line String = " << lineString << std::endl;
			servStrings.push_back(lineString);
			if (lineString.find("{") != std::string::npos)
				bracketOpen.push_back(true);
			if (lineString.find("}") != std::string::npos)
				bracketOpen.pop_back();
			if (bracketOpen.size() == 0)
			{

				servOpen = false;
				servStrings.pop_back();
				serv.push_back(Server(servStrings));
				servStrings.clear();
				i++;
			}
		}
	}
	return (0);
}

