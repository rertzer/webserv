/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stringTrim.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/03 14:28:11 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/03 15:23:45 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

void	stringTrim(std::string & str)
{
	int	pos = str.find_first_not_of(" \t");
	if (pos != -1 && pos != 0)
		str.erase(0, pos);
	pos = str.find_last_not_of(" \t");
	if (pos != -1 && str.length() && static_cast<unsigned int>(pos) != str.length() - 1)
		str.erase(pos + 1, -1);
}
