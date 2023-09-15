/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 11:53:30 by pjay              #+#    #+#             */
/*   Updated: 2023/09/15 16:22:17 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


int Server::getAllowMethods()
{
	return (_allowedMethod);
}

std::string Server::getAutoIndex()
{
	return (_autoIndex);
}

Server::Server(std::vector<std::string> servStrings)
{
	bool locOpen = false;
	std::vector<std::string> locString;
	std::vector<bool> bracOpen;
	_allowedMethod = GETPOSTDELETE;
	_maxBodySize = -1;
	for (std::vector<std::string>::iterator it = servStrings.begin(); it != servStrings.end(); it++)
	{
		if (it->find("allow_methods") != std::string::npos && locOpen == false)
		{
			_allowedMethod = getAllowMethodsServer(it->substr(it->find("allow_methods") + 14, it->find(";") - it->find("allow_methods") - 14));
		}
		else if (it->find("listen") != std::string::npos && locOpen == false)
			_nPort = atoi(it->substr(it->find("listen") + 7, it->find(";") - it->find("listen") - 7).c_str());
		else if (it->find("server_name") != std::string::npos)
			_servName = it->substr(it->find("server_name") + 12, it->find(";") - it->find("server_name") - 12);
		else if (it->find("root") != std::string::npos && locOpen == false)
			_root = it->substr(it->find("root") + 5, it->find(";") - it->find("root") - 5);
		else if (it->find("error_page") != std::string::npos)
		{
			std::string errorPage;
			std::string errorNb;
			if (it->length() > 11 + 3)
			{
				errorNb = it->substr(it->find("error_page") + 11, 3);
				if (atoi(errorNb.c_str()) < 300 || atoi(errorNb.c_str()) > 599)
				{
					std::cout << "Error nb page have to be lower than 599 and upper than 300" << std::endl;
					throw(ServerException());
				}
			}
			else
				throw(ServerException());
			if (it->find("error_page") + 15 != std::string::npos)
				errorPage = it->substr(it->find("error_page") + 15, it->find(";") - it->find("error_page") - 15);
			else
				throw(ServerException());
			_errorPage.insert(std::pair<std::string, std::string>(errorNb, errorPage));
		}
		else if (it->find("client_max_body_size") != std::string::npos)
		{
			std::string maxBodySize = it->substr(it->find("client_max_body_size") + 21, it->find(";") - it->find("client_max_body_size") - 21);
			if (checkIfOnlyDigits(maxBodySize) == 0)
				_maxBodySize = atoi(it->substr(it->find("max_body_size") + 14, it->find(";") - it->find("max_body_size") - 14).c_str());
			else
			{
				std::cout << "maxBodySize[maxBodySize.length() - 1] " << maxBodySize[maxBodySize.length() - 1] << std::endl;
				std::cout << "client_max_body_size doesn't respect subject rules" << std::endl;
				throw(ServerException());
			}
		}
		else if (it->find("autoindex") != std::string::npos && locOpen == false)
		{
			*it = it->substr(it->find("autoindex") + 10, it->find(";") - it->find("autoindex") - 10);
			if (*it == "on")
				_autoIndex = "on";
			else if (*it == "off")
				_autoIndex = "off";
			else
			{
				std::cout << "autoindex need to be on or off" << std::endl;
				throw(ServerException());
			}
		}
		else if (it->find("index ") != std::string::npos && locOpen == false)
		{
			std::string defaultPage = it->substr(it->find("index") + 6, it->find(";") - it->find("index ") - 6);
			std::stringstream ss(defaultPage);
			std::string defaultStock;
			while (getline(ss, defaultStock, ' '))
			{
				if (defaultStock != "")
					_defaultPage.push_back(defaultStock);
			}
			if (_defaultPage.size() == 0)
				throw(ServerException());
		}
		else if (it->find("location ") != std::string::npos)
			locOpen = true;
		else
		{
			if (locOpen == false)
			{
				std::cout << "line not know -> "<< *it << " <- End of line not know" << std::endl;
				throw(ServerException());
			}
		}
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
	if (_autoIndex.length() == 0)
		_autoIndex = "off";
	checkIfHaveNeccessary();
}

void Server::checkIfHaveNeccessary()
{
	int polo[] = {400, 403, 404, 405, 413, 500, 501, 505};
	if (_root.empty())
		throw (ServerException());
	for (int i = 0; i < 8; i++)
	{
  		if (_errorPage.find(intToString(polo[i])) == _errorPage.end())
			throw(ServerException());
	}
}

Server::~Server()
{
}

std::string& Server::getServName()
{
	return (_servName);
}


int Server::getBodySize() const
{
	return (_maxBodySize);
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

int & Server::getListenPort()
{
	return (_nPort);
}

std::vector<std::string>& Server::getDefaultPage()
{
	return (_defaultPage);
}

Server& Server::operator=(const Server& rhs)
{
	#ifdef DEBUG
		std::cout << "Server copy by operator called" << std::endl;
	#endif
	if (this != &rhs)
	{
		_servName = rhs._servName;
		_allowedMethod = rhs._allowedMethod;
		_autoIndex = rhs._autoIndex;
		_maxBodySize = rhs._maxBodySize;
		_root = rhs._root;
		_errorPage = rhs._errorPage;
		_location = rhs._location;
		_nPort = rhs._nPort;
		_defaultPage = rhs._defaultPage;
	}
	return (*this);
}

Server::Server(const Server& rhs)
{
	#ifdef DEBUG
		std::cout << "Server copy constructor called" << std::endl;
	#endif
	*this = rhs;
}

Server::Server()
{
	#ifdef DEBUG
		std::cout << "Server default constructor called" << std::endl;
	#endif

}

