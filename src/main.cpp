/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 11:34:55 by pjay              #+#    #+#             */
/*   Updated: 2023/08/07 15:31:40 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdio>
#include <signal.h>
#include "Server.hpp"
#include "macroDef.hpp"

volatile sig_atomic_t quitok = false;

void	handleBreak(int a)
{
	std::cout << "Breaking bad " << a << " " << SIGINT <<"\n";
	if (a == SIGINT)
		quitok = true;
}

int main(int ac, char **av)
{
	// sigaction for SINGINT
	struct sigaction	sigbreak;
	sigbreak.sa_handler = &handleBreak;
	sigemptyset(&sigbreak.sa_mask);
	sigaddset(&sigbreak.sa_mask, SIGTERM);
	sigbreak.sa_flags = 0;
	if (sigaction(SIGINT, &sigbreak, NULL) != 0)
	{
		std::perror("sigaction");
		return 1;
	}

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
