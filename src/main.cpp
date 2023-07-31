/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 11:34:55 by pjay              #+#    #+#             */
/*   Updated: 2023/07/31 10:14:46 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfFile.hpp"
#include "Server.hpp"
#include "macroDef.hpp"


int main(int ac, char **av)
{
	if (ac != 2 || std::string(av[1]).substr(std::string(av[1]).find(".")) != ".conf")
	{
		std::cout << "The program need 1 parametter, a .conf parametter" << std::endl;
		return (1);
	}
	if (checkConfFile(av[1]) == -1)
		return (1);
	std::vector<Server> serv;
	if (fillServ(av[1], serv) == -1)
		return (1);
}
