/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_test2.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:30:59 by rertzer           #+#    #+#             */
/*   Updated: 2023/07/31 17:49:22 by rertzer          ###   ########.fr       */
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
		std::cout << "fd in main " << pool.getSocketByFd(4).getFd() << std::endl;
		int i = 0;
		std::cerr << "a\n";
		while (i < 2)
		{
			std::cerr << "b\n";
			int nfds = pool.wait();
			std::cerr << "c\n";
			
			std::cout << "\nepoll collected " << nfds << " fd's. i is " << i << std::endl;
		
			for (int n = 0; n < nfds; n++)
			{
				Event 		ev = pool.nextEvent();
				TCPSocket &	soc = pool.getSocketByFd(ev.getSocketFd());

				std::cout << "n is " << n << ", fd is: " << ev.getSocketFd() << " and  events are: " << ev.getEvents() << std::endl;
				
				if (pool.isMother(ev))
				{
					i++;
					pool.connect(ev);
				}
				else
				{
					std::cout << "events on " << ev.getSocketFd() << std::endl;
					if (ev.isIn())
					{
						int readsz = soc.read();
						std::cout << "read: " << readsz << std::endl;
						std::cout << soc.getMessage() << std::endl;
					}
					if (ev.isOut())
					{
						std::ostringstream oss;
						oss << ev.getSocketFd();
						std::string msg = "Hello World! from " + oss.str();
						int sz = soc.send(msg);
						std::cout << "hello message sent by " << ev.getSocketFd() << " of size " << sz << std::endl;
						soc.close();
						pool.removeSocket(ev.getSocketFd());
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
