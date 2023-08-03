/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   splitCsv.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/03 15:43:07 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/03 15:54:19 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <string>

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
