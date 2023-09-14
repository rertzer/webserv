/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LineLoc.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 11:08:25 by pjay              #+#    #+#             */
/*   Updated: 2023/09/14 13:30:09 by pjay             ###   ########.fr       */
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
			if (_cmd != "root" && _cmd != "return" && _cmd != "autoindex" && _cmd != "allow_methods" && _cmd != "index" && _cmd != "extension" && _cmd != "cgi_path" && _cmd != "upload_path")
			{
				std::cout << "Not a valid command" << std::endl;
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
	{
		std::cout << "Only one item after an index keyword in a location" << std::endl;
		throw(ServerException());
	}
	if (_cmd == "root" && _args.size() != 1)
	{
		std::cout << "Only one item after a root keyword in a location" << std::endl;
		throw(ServerException());
	}
	if (_cmd == "upload_path" && _args.size() != 1)
	{
		std::cout << "Only one item after a upload_path keyword in a location" << std::endl;
		throw(ServerException());
	}
	if (_cmd == "return" && _args.size() != 2)
	{
		std::cout << "Only one number and one file after a retur keyword" << std::endl;
		throw(ServerException());
	}
	if (_cmd == "autoindex" && _args.size() != 1)
	{
		std::cout << "Only one item after an autoindex keyword in a location" << std::endl;
		throw(ServerException());
	}
	if (_cmd == "allow_methods" && _args.size() == 0)
	{
		std::cout << "At least one item after an allow_methods keyword in a location" << std::endl;
		throw(ServerException());
	}
	if (_cmd == "extension" && _args.size() != 1)
	{
		std::cout << "Only one item after an extension keyword in a location" << std::endl;
		throw(ServerException());
	}
	if (_cmd == "cgi_path" && _args.size() != 1)
	{
		std::cout << "Only one item after an cgi_path keyword in a location" << std::endl;
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
	for (size_t i = 0; i < _args.size(); i++)
	{
		if (_args[i] == toFind)
			return (1);
	}
	return (0);
}
