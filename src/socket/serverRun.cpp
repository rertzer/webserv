/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverRun.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:30:59 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/07 13:40:39 by rertzer          ###   ########.fr       */
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
						pool.removeSocket(ev.getSocketFd());
						pool.addMotherSocket(port);
					}
				}
				else
				{
					int	 close_fd = ev.handleEvent();
					std::cout << "close fd is " << close_fd << std::endl;
					if (close_fd == 1)
						pool.setOut(ev.getSocketFd());
					else if (close_fd == 2)
						pool.resetOut(ev.getSocketFd());
					else if (close_fd > 2)
						pool.removeSocket(close_fd);
				}
				std::cout << "Resetting " << ev.getSocketFd() << std::endl;
				pool.reset(ev.getSocketFd());
			}
				if (counter > 77)
					break;
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
