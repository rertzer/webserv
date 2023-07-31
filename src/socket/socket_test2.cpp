/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_test2.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:30:59 by rertzer           #+#    #+#             */
/*   Updated: 2023/07/31 12:30:22 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <sys/epoll.h>
#include <iostream>

#include "TCPSocket.hpp"

int	main()
{
	try
	{
		TCPSocket	mother_socket(8080);

		std::cout << "server_id is : " << mother_socket.getFd() << std::endl;
	
		struct epoll_event ev, events[42];
		ev.events = EPOLLIN;
		ev.data.fd = mother_socket.getFd();

		int	epfd = epoll_create(42);
		if (epfd == -1)
		{
			std::cout << "BAD!! epoll_create failed\n";
			return 1;
		}

		if (epoll_ctl(epfd, EPOLL_CTL_ADD, mother_socket.getFd(), &ev) == -1)
		{
			std::cout << "BAD!! epoll_ctl failed\n";
			mother_socket.~TCPSocket();
			close(epfd);
			return 1;
		}
		int i = 0;
		TCPSocket connect;
		while (i < 2)
		{
			int nfds = epoll_wait(epfd, events, 42, -1);
			if (nfds == -1)
			{
				std::cout << "BAD!! epoll_wait failed\n";
				mother_socket.~TCPSocket();
				close(epfd);
				return 1;
			}
			std::cout << "\nepoll collected " << nfds << " fd's. i is " << i << std::endl;
			for (int n = 0; n < nfds; n++)
			{
				std::cout << "n is " << n << ", fd is: " << events[n].data.fd << " and  events are: " << events[n].events << std::endl;
				if (events[n].data.fd == mother_socket.getFd())
				{
					i++;
					mother_socket.accept(connect);
					std::cout << "new connect fd is " << connect.getFd() << std::endl;
					ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
					ev.data.fd = connect.getFd();
					if (epoll_ctl(epfd, EPOLL_CTL_ADD, connect.getFd(), &ev) == -1)
					{
						std::cout << "BAD!! epoll_ctl  2 failed\n";
						mother_socket.~TCPSocket();
						connect.close();
						close(epfd);
						return 1;
					}
				}
				else
				{
					std::cout << "events on " << events[n].data.fd << std::endl;
					if (events[n].events & EPOLLIN)
					{
						int readsz = connect.read();
						std::cout << "read: " << readsz << std::endl;
						std::cout << connect.getMessage() << std::endl;
					}
					if (events[n].events & EPOLLOUT)
					{
						std::ostringstream oss;
						oss << events[n].data.fd;
						std::string msg = "Hello World! from " + oss.str();
						int sz = connect.send(msg);
						std::cout << "hello message sent by " << events[n].data.fd << " of size " << sz << std::endl;
						connect.close();
					}
					else
					{
						std::cout << "other event: " << events[n].events << std::endl;
					}
				}
			}
		}
		close(epfd);
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}


	return 0;
}
