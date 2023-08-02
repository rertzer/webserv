/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_test2.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:30:59 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/02 12:13:55 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <sys/epoll.h>
#include <iostream>

#include "Polling.hpp"

int	main()
{
	try
	{
		Polling pool;
		pool.addMotherSocket(8080);
		int i = 0;
		while (i < 8)
		{
			std::cout << "Waiting...\n";
			int nfds = pool.wait();
			
			std::cout << "\n\n\nepoll collected " << nfds << " fd's. i is " << i << std::endl;
	
			for (int n = 0; n < nfds; n++)
			{
				Event 		ev = pool.nextEvent();
				TCPSocket *	soc = pool.getSocketByFd(ev.getSocketFd());

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
					i++;
					if (ev.isIn())
						pool.connect(ev);
				}
				else
				{
					std::cout << "events on " << ev.getSocketFd() << std::endl;
					if (ev.isIn())
					{
						std::cout << "isIn event\n";
						int readsz = soc->read();
						std::cout << "read: " << readsz << std::endl;
						std::cout << soc->getMessage() << std::endl;
					}
					if (ev.isOut())
					{
						std::ostringstream oss;
						oss << ev.getSocketFd();
						std::string msg = "Hello World! from " + oss.str();
						int sz = soc->send(msg);
						std::cout << "hello message sent by " << ev.getSocketFd() << " of size " << sz << std::endl;
						//soc->close();
						//pool.removeSocket(ev.getSocketFd());
					}
					else
					{
						std::cout << "other event: " << ev.getEvents() << std::endl;
					}
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
