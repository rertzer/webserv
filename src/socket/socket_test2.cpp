/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_test2.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:30:59 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/03 14:54:21 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <sys/epoll.h>
#include <iostream>

#include "Polling.hpp"
#include "Server.hpp"

int	testSocket(std::vector<Server> serv)
{
	try
	{
		Polling pool;
		std::map<int, int>	unique_port;
		for (std::vector<Server>::iterator it = serv.begin(); it != serv.end(); it++)
		{
			std::vector<int>	vp = it->getListenPort();
			for (std::vector<int>::iterator jt = vp.begin(); jt != vp.end(); jt++)
				unique_port[*jt] = 1;
		}
		for (std::map<int, int>::iterator it = unique_port.begin(); it != unique_port.end(); it++)
			pool.addMotherSocket(it->first);

		int i = 1;
		while (i)
		{
			i++;
			std::cout << "Waiting...\n";
			int nfds = pool.wait();

			std::cout << "\n\n\nepoll collected " << nfds << " fd's. i is " << i << std::endl;

			for (int n = 0; n < nfds; n++)
			{
				Event 		ev = pool.nextEvent();
				ev.setServ(serv);

				std::cout << "\nn is " << n << ", fd is: " << ev.getSocketFd() << " and  events are: " << ev.getEvents() << std::endl;
				if (ev.getEvents() & EPOLLIN)
					std::cout << "EPOLLIN\n";
				if (ev.getEvents() & EPOLLOUT)
					std::cout << "EPOLLOUT\n";
				if (ev.getEvents() & EPOLLRDHUP)
					std::cout << "EPOLLRDHUP\n";
				if (ev.getEvents() & EPOLLPRI)
					std::cout << "EPOLLPRI\n";
				if (ev.getEvents() & EPOLLERR)
					std::cout << "EPOLLERR\n";
				if (ev.getEvents() & EPOLLHUP)
					std::cout << "EPOLLHUP\n";
				if (ev.getEvents() & EPOLLET)
					std::cout << "EPOLLET\n";
				if (ev.getEvents() & EPOLLONESHOT)
					std::cout << "EPOLLONESHOT\n";

				if (pool.isMother(ev))
				{
					if (ev.isIn())
						pool.connect(ev);
				}
				else
				{
					std::cout << "events on " << ev.getSocketFd() << std::endl;

					ev.handleEvent();
				}
			}
		}
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
