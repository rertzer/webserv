/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 11:53:30 by pjay              #+#    #+#             */
/*   Updated: 2023/07/31 11:42:37 by pjay             ###   ########.fr       */
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
	// std::cout << "Server side : " << serv.getServSide().sin_addr.s_addr << std::endl;
	// std::cout << "Server side : " << serv.getServSide().sin_port << std::endl;
	// std::cout << "Server side : " << serv.getServSide().sin_family << std::endl;
	// std::cout << "Server side : " << serv.getServSide().sin_zero << std::endl;
	std::cout << "Server root : " << serv.getRoot() << std::endl;
	std::cout << "Server error page : " << std::endl;
	for (std::map<std::string, std::string>::iterator it = serv.getAllErrorPage().begin(); it != serv.getAllErrorPage().end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;
}

Server::Server(std::vector<std::string> servStrings)
{
	for (std::vector<std::string>::iterator it = servStrings.begin(); it != servStrings.end(); it++)
	{
		if (it->find("server_name") != std::string::npos)
			_servName = it->substr(it->find("server_name") + 12, it->find(";"));
		if (it->find("root") != std::string::npos)
			_root = it->substr(it->find("root") + 5, it->find(";"));
		if (it->find("error_page") != std::string::npos)
		{
			std::string errorNb = it->substr(it->find("error_page") + 11, 3);
			std::string errorPage = it->substr(it->find("error_page") + 15, it->find(";"));
			_errorPage.insert(std::pair<std::string, std::string>(errorNb, errorPage));
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

int& Server::getServSocket()
{
	return (_servSocket);
}

struct sockaddr_in& Server::getServSide()
{
	return (_servSide);
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

std::map<std::string, std::string>& Server::getAllLocation()
{
	return (_location);
}
