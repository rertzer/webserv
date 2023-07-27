/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:30:59 by rertzer           #+#    #+#             */
/*   Updated: 2023/07/27 16:14:00 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

int	main()
{
	struct sockaddr_in	server_addr, peer_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	int	server_id = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	
	if (server_id == 0)
	{
		std::cout << "BAD!!! socket failed\n";
		return 1;
	}

	if (bind(server_id, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == -1)
	{
		std::cout << "BAD!! bind failed\n";
		close(server_id);
		return 1;
	}
	if (listen(server_id, 42) == -1)
	{
		std::cout << "BAD!! listen failed\n";
		close(server_id);
		return 1;
	}

	std::cout << "server_id is : " << server_id << std::endl;
	
	socklen_t	peer_addr_size = sizeof(peer_addr);

	struct epoll_event ev, events[42];
	ev.events = EPOLLIN;
	ev.data.fd = server_id;

	int	epfd = epoll_create(42);
	if (epfd == -1)
	{
		std::cout << "BAD!! epoll_create failed\n";
		close(server_id);
		return 1;
	}


	if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_id, &ev) == -1)
	{
		std::cout << "BAD!! epoll_ctl failed\n";
		close(server_id);
		close(epfd);
		return 1;
	}
	int i = 0;
	while (i < 16)
	{
		int nfds = epoll_wait(epfd, events, 42, -1);
		if (nfds == -1)
		{
			std::cout << "BAD!! epoll_wait failed\n";
			close(server_id);
			close(epfd);
			return 1;
		}
		std::cout << "epoll collected " << nfds << " fd's. i is " << i << std::endl;
		for (int n = 0; n < nfds; n++)
		{
			int	connect_fd = -1;
			std::cout << "n is " << n << ", fd is: " << events[n].data.fd << " and  events are: " << events[n].events << std::endl;
			if (events[n].data.fd == server_id)
			{
				i++;
				connect_fd = accept(server_id, reinterpret_cast<struct sockaddr*>(&peer_addr), &peer_addr_size);
				if (connect_fd == -1)
				{
					std::cout << "BAD!! accept failed\n";
					close(server_id);
					close(epfd);
					return 1;
				}
				else
				{
					std::cout << "new connect fd is " << connect_fd << std::endl;
				}
				//setnonblocking(connect_fd);
				ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
				ev.data.fd = connect_fd;
				if (epoll_ctl(epfd, EPOLL_CTL_ADD, connect_fd, &ev) == -1)
				{
					std::cout << "BAD!! epoll_ctl  2 failed\n";
					close(server_id);
					close(connect_fd);
					close(epfd);
					return 1;
				}
			}
			else
			{
				std::cout << "events on " << events[n].data.fd << std::endl;
				if (events[n].events & EPOLLIN)
				{
					char buffer[1024] = {0};
					int	readsz = read(events[n].data.fd, buffer, 1024);
					std::cout << "read: " << readsz << std::endl;
					std::cout << buffer << std::endl;
				}
				if (events[n].events & EPOLLOUT)
				{
					std::ostringstream oss;
					oss << events[n].data.fd;
					std::string msg = "Hello World! from " + oss.str();
					int sz = send(events[n].data.fd, msg.c_str(), msg.length(), 0);
					std::cout << "hello message sent by " << events[n].data.fd << " of size " << sz << std::endl;
					
				}
				else
				{
					std::cout << "other event: " << events[n].events << std::endl;
				}
			}
		}
	}


	close(server_id);
	close(epfd);

	return 0;
}
