/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkAndGet.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 13:43:09 by pjay              #+#    #+#             */
/*   Updated: 2023/09/19 11:49:20 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "TCPSocket.hpp"

void checkExec(std::string filePath)
{
	if (access(filePath.c_str(), F_OK) == -1)
	{
		throw (ErrorException(404));
	}
	if (access(filePath.c_str(), R_OK) == -1)
	{
		throw (ErrorException(403));
	}
}

int CheckForRedirection(Location loc)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	while (it != lineLoc.end())
	{
		if (it->getCmd() == "return")
			return (1);
		it++;
	}
	return (0);
}

std::string getSpecIndex(Location loc, Response &rep)
{
	std::vector<std::string> index = loc.getIndex();
	if (index.size() == 0)
		return ("");
	else
	{
		for (std::vector<std::string>::iterator it = index.begin() ; it != index.end(); it++)
		{
			if (access((rep.getRoot() + *it).c_str(), F_OK) != -1 && access((rep.getRoot() + *it).c_str(), R_OK) != -1)
				return (*it);
		}
		return ("");
	}
}

Location getTheLocation(std::string path, Response &rep)
{
	std::vector<Location> loc = rep.getServ().getAllLocation();
	if (path != "/")
	{
		path = path.substr(0, path.rfind("."));
		path = path.substr(0, path.rfind("/"));
	}
	while (path.find("/") != std::string::npos && path != "/")
	{
		for(std::vector<Location>::iterator it = loc.begin();it != loc.end(); it++)
		{
			if (it->getLocationPath() == path)
			{
				std::cout << "found a path " << std::endl;
				return (*it);
			}
		}
		path = path.substr(0, path.rfind("/"));
		std::cout << "Path = " << path << std::endl;
 	}
	return (Location());
}

int checkIfLocation(std::string path, Response &rep)
{
	std::vector<Location> loc = rep.getServ().getAllLocation();

	if (path != "/")
	{
		path = path.substr(0, path.rfind("."));

	}
	std::cout << path << "  path " << std::endl;
	while (path.find("/") != std::string::npos && path != "/")
	{
		for(std::vector<Location>::iterator it = loc.begin();it != loc.end(); it++)
		{
			if (it->getLocationPath() == path)
			{
				std::cout << "found a path " << std::endl;
				return (0);
			}
		}
		path = path.substr(0, path.rfind("/"));
		std::cout << "Path = " << path << std::endl;
 	}
	return (-1);
}


