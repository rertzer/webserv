/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 11:34:55 by pjay              #+#    #+#             */
/*   Updated: 2023/07/26 15:57:46 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfFile.hpp"

int main(int ac, char **av)
{
	if (ac != 2 || std::string(av[1]).substr(std::string(av[1]).find(".")) != ".conf")
	{
		std::cout << "The program need 1 parametter, a .conf parametter" << std::endl;
	}
	ConfFile confFile(av[1]);
	std::cout << "Name of the server is " << confFile.getServName() << std::endl;
}
