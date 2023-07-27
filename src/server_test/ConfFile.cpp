/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfFile.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 11:44:12 by pjay              #+#    #+#             */
/*   Updated: 2023/07/26 16:44:57 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ConfFile.hpp"

ConfFile::ConfFile()
{
	#ifdef DEBUG
		std::cout << "ConfFile default constructor called" << std::endl;
	#endif
}

ConfFile::ConfFile(const ConfFile& rhs)
{

	#ifdef DEBUG
		std::cout << "ConfFile copy constructor called" << std::endl;
	#endif
		*this = rhs;
}

ConfFile ConfFile::operator=(const ConfFile& rhs)
{

		#ifdef DEBUG
			std::cout << "ConfFile copy by operator called" << std::endl;
		#endif
		if (this != &rhs)
		{
				this->_servSide = rhs._servSide;
				this->_servName = rhs._servName;
		}
		return (*this);
}

ConfFile::~ConfFile()
{
	#ifdef DEBUG
		std::cout << "ConfFile destructor called" << std::endl;
	#endif
}

std::string ConfFile::getServName()
{
	return (this->_servName);
}

struct sockaddr_in ConfFile::getServSide()
{
	return (this->_servSide);
}

int ConfFile::setServName(char *av)
{
	std::ifstream conf;
	std::string fullText;
	size_t pos;
	bool found = false;
	std::string servString;
	try
	{
		conf.open(av, std::fstream::in);
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		return (-1);
	}
	while (getline(conf, fullText))
	{
		pos = fullText.find ("server_name");
		if (pos == std::string::npos)
			continue;
		if (fullText.substr(pos + 11)[0] != ' ') // 12 equal to server_name len
			continue;
		servString = fullText.substr(pos + 1);
		_servName = servString.substr(11, servString.length() - 12);
		found = true;
		break;
	}
	if (found == false)
		return (-1);
	else
		return (0);
}

int ConfFile::setServSide(char *av)
{
	(void)av;
	return (0);
}


ConfFile::ConfFile(char *av)
{
	if (setServName(av) == -1)
	{
		std::cout << "Serv name is not good" << std::endl;
		return ;
	}
	if (setServSide(av) == -1)
	{
		std::cout << "Conf file is not good" << std::endl;
	}
}
