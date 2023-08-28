/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validConfFile.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 09:54:11 by pjay              #+#    #+#             */
/*   Updated: 2023/08/28 11:13:27 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "macroDef.hpp"
#include "Server.hpp"

static int checkBracket(std::ifstream& conf);

int checkConfFile(std::string av)
{
	std::ifstream conf;
	conf.open(av.c_str(), std::fstream::in);
	if (!conf.is_open())
	{
		std::cout << "The file " << av << " doesn't exist" << std::endl;
		return (-1);
	}
	if (checkBracket(conf) == 0)
	{
		conf.close();
		return (0);
	}
	else
	{
		conf.close();
		return (-1);
	}
}

//uggly function to check the parsing of the config file
// need to be cleaner
int checkBracket(std::ifstream& conf)
{
	std::vector<bool> bracketOpen;
	std::string fullText;
	int line = 0;
	while (getline(conf, fullText))
	{
		line++;
		bool fullOfWs = true;
		if (fullText.find("#") != std::string::npos)
			fullText = fullText.substr(0, fullText.find("#"));
		for (size_t i = 0; i < fullText.size(); i++)
		{
			if (isspace(fullText[i]) == false)
			{
				fullOfWs = false;
			}
		}
		if (fullOfWs == true || fullText.empty() == true)
			continue;
		if (fullText.find(";") != std::string::npos)
		{
			fullText = fullText.substr(0, fullText.find(";") + 1);
			if (fullText[fullText.size() - 1] != ';')
			{
				CFNG_SMI_COLON;
				return (-1);
			}
		}
		else if ((fullText.find("{") == std::string::npos) && (fullText.find("}") == std::string::npos))
		{
			std::cout << "Line where ';' is missing: " << fullText << " line = " << line << std::endl;
			CFNG_SMI_COLON;
			return (-1);
		}
		if (fullText.find("{") != std::string::npos)
			bracketOpen.push_back(true);
		if (fullText.find("}") != std::string::npos)
		{
			if (bracketOpen.size() > 0)
				bracketOpen.pop_back();
			else
			{
				CFNG_BRAC_CLOSE;
				return (-1);
			}
		}
	}
	if (bracketOpen.size() == 0)
	{
		//std::cout << "Brackets are well closed" << std::endl;
		return (0);
	}
	else
	{
		CFNG_BRAC_OPEN;
		return (-1);
	}
}
