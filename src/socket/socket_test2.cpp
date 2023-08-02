/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_test2.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:30:59 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/02 18:19:32 by rertzer          ###   ########.fr       */
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
