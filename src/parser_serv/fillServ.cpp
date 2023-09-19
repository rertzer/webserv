/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fillServ.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 09:44:50 by pjay              #+#    #+#             */
/*   Updated: 2023/09/19 12:58:45 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void printVec(std::vector<std::string> vec)
{
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
		std::cout << *it << std::endl;
}

int checkPort(std::vector<Server>& serv)
{
	for (std::vector<Server>::iterator it = serv.begin(); it != serv.end(); it++)
	{
		for (std::vector<Server>::iterator it2 = serv.begin(); it2 != serv.end(); it2++)
		{
			if (it != it2)
			{
				if (it2->getListenPort() == it->getListenPort())
				{
					if (it2->getServName() == it->getServName())
					{
						std::cout << "If serv have the same port they have to have a different name" << std::endl;
						return (-1);
					}
				}
			}
		}
	}
	return (0);
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
		if (lineString.find_first_not_of(" \t\r") == std::string::npos)
			continue;
		if (lineString.find("server {") != std::string::npos) // I want it to skip the server line
		{
			servOpen = true;
			bracketOpen.push_back(true);
			continue;
		}
		if (servOpen == true)
		{
			if (lineString.find_first_not_of(" \t\n\r") != std::string::npos)
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
	if (checkPort(serv) == -1)
		return (-1);

	return (0);
}

