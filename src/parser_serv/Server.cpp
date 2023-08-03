/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 11:53:30 by pjay              #+#    #+#             */
/*   Updated: 2023/08/03 10:51:59 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// Server::Server()
// {
// }

// Server::Server(std::vector<std::string> servStrings)
// {
// 	(void)servStrings;
// }

// Server::Server(const Server& rhs)
// {
// 	(void)rhs;
// }

void printServ(Server& serv)
{
	std::cout << "Server name : " << serv.getServName() << std::endl;
	std::cout << "Server root : " << serv.getRoot() << std::endl;
	std::cout << "Server error page : " << std::endl;
	for (std::vector<int>::iterator it = serv.getListenPort().begin(); it < serv.getListenPort().end(); it++)
		std::cout << "Listening on port : " << *it  << std::endl;
	for (std::map<std::string, std::string>::iterator it = serv.getAllErrorPage().begin(); it != serv.getAllErrorPage().end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;

	std::cout << "--------Location-------------" << std::endl;

	std::vector<Location> loc = serv.getAllLocation();
	for (std::vector<Location>::iterator it = loc.begin(); it != loc.end(); it++)
	{
		std::cout << "Location path : " << it->getLocationPath() << std::endl;
		std::cout << "Location line : " << std::endl;
		it->printLoc();
	}
}

Server::Server(std::vector<std::string> servStrings)
{
	bool locOpen = false;
	std::vector<std::string> locString;
	std::vector<bool> bracOpen;
	std::cout << "size of servString vector = " << servStrings.size() << std::endl;
	for (std::vector<std::string>::iterator it = servStrings.begin(); it != servStrings.end(); it++)
	{
		std::cout << *it << std::endl;
		if (it->find("listen") != std::string::npos)
			_nPort.push_back(atoi(it->substr(it->find("listen") + 7, it->find(";") - it->find("listen") - 7).c_str()));
		if (it->find("server_name") != std::string::npos)
			_servName = it->substr(it->find("server_name") + 12, it->find(";") - it->find("server_name") - 12);
		if (it->find("root") != std::string::npos)
			_root = it->substr(it->find("root") + 5, it->find(";") - it->find("root") - 5);
		if (it->find("error_page") != std::string::npos)
		{
			std::string errorNb = it->substr(it->find("error_page") + 11, 3);
			std::string errorPage = it->substr(it->find("error_page") + 15, it->find(";") - it->find("error_page") - 15);
			_errorPage.insert(std::pair<std::string, std::string>(errorNb, errorPage));
		}
		if (it->find("index ") != std::string::npos)
		{
			std::string defaultPage = it->substr(it->find("index ") + 6, it->find(";") - it->find("index ") - 6);
			std::stringstream ss(defaultPage);
			std::string defaultStock;
			while (getline(ss, defaultStock, ' '))
			{
				if (defaultStock != "")
					_defaultPage.push_back(defaultStock);
			}
		}
		if (it->find("location ") != std::string::npos)
			locOpen = true;
		if (locOpen == true)
		{
			locString.push_back(*it);
			if (it->find("{") != std::string::npos)
				bracOpen.push_back(true);
			if (it->find("}") != std::string::npos)
				bracOpen.pop_back();
			if (bracOpen.size() == 0)
			{
				locOpen = false;
				_location.push_back(Location(locString));
				locString.clear();
			}
		}
	}
	printServ(*this);
}

Server::~Server()
{
}

std::string& Server::getServName()
{
	return (_servName);
}

std::string& Server::getRoot()
{
	return (_root);
}

std::string Server::getErrorPage(std::string errorNb)
{
	if (_errorPage.find(errorNb) == _errorPage.end())
		return (_errorPage.find("404")->second);
	return (_errorPage.find(errorNb)->second);
}

std::map<std::string, std::string>& Server::getAllErrorPage()
{
	return (_errorPage);
}

std::vector<Location>& Server::getAllLocation()
{
	return (_location);
}

std::vector<int>& Server::getListenPort()
{
	return (_nPort);
}
