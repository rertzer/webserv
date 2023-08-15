/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   splitCsv.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/03 15:43:07 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/15 09:28:51 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <string>
#include <locale>

#include "macroDef.hpp"

std::vector<std::string>	splitCsv(std::string const & str)
{
	int	start = 0;
	int	end = -1;
	std::vector<std::string>	splited;
	
	while (end)
	{
		end = str.find(",", start);
		std::string field = str.substr(start, end - start);
		stringTrim(field);
		splited.push_back(field);
		start = ++end;
	}
	return splited;
}

bool	ciCompare(std::string const & left, std::string const & right)
{
	std::string	ci_left = lowString(left);
	std::string	ci_right = lowString(right);
	if (ci_left.compare(ci_right))
		return false;
	return true;
}	
	
std::string	lowString(std::string const & str)
{
	std::locale loc;
	std::string	ci_string;

	for (std::string::size_type i = 0; i <str.length(); ++i)
	{
		std::string tmp = "_";
		tmp[0] = std::tolower(str[i], loc);
		ci_string.append(tmp);
	}
	return ci_string;
}
