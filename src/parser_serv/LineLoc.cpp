/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LineLoc.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 11:08:25 by pjay              #+#    #+#             */
/*   Updated: 2023/08/16 13:52:41 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LineLoc.hpp"

LineLoc::LineLoc(std::string line)
{
	std::stringstream ss(line);
	std::string defaultStock;
	int i = 0;
	while (getline(ss, defaultStock, ' '))
	{
		if (i == 0)
		{
			_cmd = defaultStock.substr(defaultStock.find_first_not_of(" \t"), defaultStock.length());
		}
		else if (defaultStock != "")
		{
			if (defaultStock.find(";") != std::string::npos && defaultStock.find(";") == defaultStock.length() - 1)
				defaultStock = defaultStock.substr(0, defaultStock.find(";"));
			_args.push_back(defaultStock);
		}
		i++;
	}
}

std::string& LineLoc::getCmd()
{
	return (_cmd);
}

std::vector <std::string>& LineLoc::getArgs()
{
	return (_args);
}

int LineLoc::checkArgs(std::string toFind)
{
	std::cout << "toFind = " << toFind << std::endl;
	for (size_t i = 0; i < _args.size(); i++)
	{
		std::cout << "args = " << _args[i] << std::endl;
		if (_args[i] == toFind)
			return (1);
	}
	return (0);
}
