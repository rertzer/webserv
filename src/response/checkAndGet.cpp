/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkAndGet.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 13:43:09 by pjay              #+#    #+#             */
/*   Updated: 2023/09/13 10:12:49 by pjay             ###   ########.fr       */
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
	std::vector<Location>::iterator it = loc.begin();
	if (path != "/")
	{
		path = path.substr(0, path.rfind("."));
		path = path.substr(0, path.rfind("/"));
	}
	while (it != loc.end())
	{
		if (it->getLocationPath() == path)
			return (*it);
		it++;
	}
	return (Location());
}

int checkIfLocation(std::string path, Response &rep)
{
	std::vector<Location> loc = rep.getServ().getAllLocation();
	std::vector<Location>::iterator it = loc.begin();
	if (path != "/")
	{
		path = path.substr(0, path.rfind("."));
		path = path.substr(0, path.rfind("/"));
	}
	while (it != loc.end())
	{
		if (it->getLocationPath() == path)
		{
			return (0);
		}
		it++;
	}
	return (-1);
}