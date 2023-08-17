/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 15:33:20 by pjay              #+#    #+#             */
/*   Updated: 2023/08/16 13:44:23 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"


Location::Location(std::vector<std::string> locString)
{
	size_t line = 0;

	for (std::vector<std::string>::iterator it = locString.begin(); it != locString.end(); it++)
	{

		if (line == 0)
			_locationPath = it->substr(it->find("location") + 9, it->find("{") - it->find("location") - 10);
		else if (*it != "" && it->find("}") == std::string::npos)
		{
			_locationLine.push_back((LineLoc)*it);
		}
		line++;
	}
}

std::string Location::getLocationPath() const
{
	return (_locationPath);
}

std::vector<LineLoc>& Location::getLocationLine()
{
	return (_locationLine);
}


Location::Location()
{
}

void Location::printLoc()
{
	for (std::vector<LineLoc>::iterator it = _locationLine.begin(); it != _locationLine.end(); it++)
	{
		std::cout << "Line cmd = " << it->getCmd() << std::endl;
		for (std::vector<std::string>::iterator it2 = it->getArgs().begin(); it2 != it->getArgs().end(); it2++)
			std::cout << "Value = " << *it2 << std::endl;
	}
}
