/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverRun.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:30:59 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/10 09:38:28 by rertzer          ###   ########.fr       */
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
		std::cout << "Listening...\n";
		int counter = 0;
		while (1)
		{
			int rc = pool.wait();
			counter++;
			std::cout << "loop " << counter << std::endl;
			if (quitok)
			{
				std::cout << "quitting the loop\n";
				break;
			}

			for (int n = 0; n < rc; n++)
			{
				Event 		ev = pool.nextEvent();
				ev.setServ(serv);

				if (pool.isMother(ev))
				{
					if (ev.isIn())
					{
						pool.connect(ev);
					}

					std::string	event_msg;
					if (ev.isErr())
						event_msg += "EPOLLERR ";
					if (ev.isHup())
						event_msg += "EPOLLHUP ";
					if (!event_msg.empty())
					{
						int port = ev.getSocket()->getPort();
						std::cout << event_msg << ". Restarting connection on port " << port << std::endl;
						pool.removeSocket(ev.getFd());
						pool.addMotherSocket(port);
					}
				}
				else
				{
					ev.handleEvent();
					std::cout << "event status is " << ev.getStatus() << std::endl;
					if (ev.getStatus() == 1)
						pool.setOut(ev.getFd());
					else if (ev.getStatus() == 2)
						pool.resetOut(ev.getFd());
					else if (ev.getStatus() == 3)
						pool.removeSocket(ev.getFd());
					else if (ev.getStatus() == 4)
						pool.addCgiFds(ev.getSocket());
					else if (ev.getStatus() == 6)
					{
						pool.removeCgiFd(ev.getFd());
						pool.setOut(ev.getSocket()->getFd());
					}
					else if (ev.getStatus() == 7)
					{
						pool.setCgiIn(ev.getSocket());
						pool.removeCgiFd(ev.getFd());
						ev.cgiExec();
					}
					else if (ev.getStatus() == 8)
					{
						std::cout << "serverRun 102\n";
						pool.addCgiFds(ev.getSocket());
						ev.cgiExec();
					}
				}
				std::cout << "Resetting " << ev.getFd() << std::endl;
				pool.reset(ev.getFd());
			}
		}
	}
	catch (const TCPSocket::SocketException & e)
	{
		std::cout << "Socket exception!!!\n";
		std::cerr << e.what() << std::endl;
	}
	catch (const Polling::PollingException & e)
	{
		std::cout << "PollingException!!!\n";
		std::cerr << e.what() << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << "Standard exception!!!\n";
		std::cerr << e.what() << std::endl;
	}
	std::cout << "After catch\n";
	return 0;
}
