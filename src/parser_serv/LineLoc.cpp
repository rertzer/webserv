/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LineLoc.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 11:08:25 by pjay              #+#    #+#             */
/*   Updated: 2023/08/26 11:30:20 by rertzer          ###   ########.fr       */
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
			if (defaultStock.find_first_not_of(" \t") == std::string::npos)
				throw(ServerException());
			_cmd = defaultStock.substr(defaultStock.find_first_not_of(" \t"), defaultStock.length());
			if (_cmd != "root" && _cmd != "return" && _cmd != "autoindex" && _cmd != "allow_methods" && _cmd != "index")
			{
				//std::cout << "cmd = " << _cmd << std::endl;
				//std::cout << "Here 1 " << std::endl;
				throw(ServerException());
			}
		}
		else if (defaultStock != "")
		{
			defaultStock = defaultStock.substr(defaultStock.find_first_not_of(" \t"), defaultStock.length());
			if (defaultStock == ";")
				break;
			if (defaultStock.find(";") != std::string::npos && defaultStock.find(";") == defaultStock.length() - 1)
				defaultStock = defaultStock.substr(0, defaultStock.find(";"));
			_args.push_back(defaultStock);
		}
		i++;
	}
	if (_cmd == "index" && _args.size() != 1)
		throw(ServerException());
	if (_cmd == "root" && _args.size() != 1)
	{
		//std::cout << "Here 2 " << std::endl;
		throw(ServerException());
	}
	if (_cmd == "return" && _args.size() != 2)
	{
		//std::cout << "Here 3 " << std::endl;
		throw(ServerException());
	}
	if (_cmd == "autoindex" && _args.size() != 1)
	{
		//std::cout << "Here 4" << std::endl;
		throw(ServerException());
	}
	if (_cmd == "allow_methods" && _args.size() == 0)
	{
		//std::cout << "line = " << defaultStock << std::endl;
		//std::cout << "Here 5 " << std::endl;
		throw(ServerException());
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
	//std::cout << "toFind = " << toFind << std::endl;
	for (size_t i = 0; i < _args.size(); i++)
	{
	//	std::cout << "args = " << _args[i] << std::endl;
		if (_args[i] == toFind)
			return (1);
	}
	return (0);
}
