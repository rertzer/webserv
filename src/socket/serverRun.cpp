/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverRun.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:30:59 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/26 11:32:07 by rertzer          ###   ########.fr       */
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
#include "Status.hpp"
#include "ErrorException.hpp"

extern sig_atomic_t	quitok;

int	serverRun(std::vector<Server> serv)
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

		while (1)
		{
			//std::cout << "Listening...\n";
			int nfds = pool.wait();
			if (quitok)
			{
				std::cout << "quitting the loop\n";
				break;
			}

			//std::cout << "\n\n\nepoll collected " << nfds << " fd's\n";

			for (int n = 0; n < nfds; n++)
			{
				Event 		ev = pool.nextEvent();
				ev.setServ(serv);

				//std::cout << "\nn is " << n << ", fd is: " << ev.getSocketFd() << " and  events are: " << ev.getEvents() << std::endl;

				if (pool.isMother(ev))
				{
					if (ev.isIn())
						pool.connect(ev);
					std::string	event_msg;
					if (ev.isErr())
						event_msg += "EPOLLERR ";
					if (ev.isHup())
						event_msg += "EPOLLHUP ";
					if (!event_msg.empty())
					{
						int port = ev.getSocket()->getPort();
						std::cerr << event_msg << ".Restarting connection on port " << port << std::endl;
						pool.removeSocket(ev.getSocketFd());
						pool.addMotherSocket(port);
					}
				}
				else
				{
					//std::cout << "events on " << ev.getSocketFd() << "mport: " << ev.getSocket()->getMotherPort() << std::endl;

					int	 close_fd = ev.handleEvent();
					if (close_fd)
						pool.removeSocket(close_fd);
				}
			}
		}
	}
	catch (const TCPSocket::SocketException & e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
