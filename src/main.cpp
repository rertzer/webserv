/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 11:34:55 by pjay              #+#    #+#             */
/*   Updated: 2023/08/03 11:03:19 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "macroDef.hpp"


int main(int ac, char **av)
{
	// Quand pas de param il faut fournir un fichier par defaut
	if (ac != 2)
	{
		std::cout << "The program need 1 parametter, a .conf parametter" << std::endl;
		return (1);
	}
	std::string arg(av[1]);
	std::string confExtension = ".conf";
	size_t extensionPos = arg.length() - confExtension.length();
	if (arg.compare(extensionPos, confExtension.length(), confExtension) != 0)
	{
		std::cout << "The program needs a .conf parameter" << std::endl;
		return 1;
	}
	if (checkConfFile(av[1]) == -1)
		return (1);
	std::vector<Server> serv;
	if (fillServ(av[1], serv) == -1)
		return (1);
	std::cout << "-------------TEST SOCKET------------------" << std::endl << std::endl;
	testSocket(serv);
}
