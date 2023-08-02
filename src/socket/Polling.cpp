/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polling.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 10:06:08 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/02 16:55:14 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Polling.hpp"

//Public
Polling::Polling():epoll_fd(0), events_nb(0), next_event(-1)
{
	//parameter unused; historical reason
	epoll_fd = ::epoll_create(42);
	if (epoll_fd == -1)
		throw (PollingException());
}

Polling::~Polling()
{
	if (epoll_fd)
	{
		close(epoll_fd);
		epoll_fd = 0;
	}
	for (std::map<int, TCPSocket*>::iterator it = powerstrip.begin(); it != powerstrip.end(); it++)
	{
		delete (it->second);
	}
}

Polling	&	Polling::operator=(Polling const & rhs)
{
	if (this != &rhs)
	{
		epoll_fd = rhs.epoll_fd;
		events_nb = rhs.events_nb;
		next_event = rhs.next_event;
		mother_fds = rhs.mother_fds;
		powerstrip = rhs.powerstrip;
		for (int i = 0; i < 42; i++)
			events[i] = rhs.events[i];
	}
	return *this;
}

void	Polling::addMotherSocket(int port)
{
	TCPSocket *	soc = new TCPSocket(port);
	addSocket(soc, EPOLLIN | EPOLLET);
	mother_fds.push_back(soc->getFd());
}

void	Polling::connect(Event const & ev)
{
	TCPSocket *	soc = new TCPSocket();
	powerstrip[ev.getSocketFd()]->accept(*soc);
	addSocket(soc, EPOLLIN | EPOLLOUT | EPOLLET);
	std::cout << "New connection fd: " << soc->getFd() << std::endl;
}

void	Polling::removeMotherSocket(int fd)
{
	mother_fds.remove(fd);
	removeSocket(fd);
}

void	Polling::removeSocket(int fd)
{
	if (::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL))
		throw (PollingException());
	delete (powerstrip[fd]);
	powerstrip.erase(fd);
}

int	Polling::wait()
{
	events_nb = ::epoll_wait(epoll_fd, events, 42, -1);
	if (events_nb == -1)
		throw (PollingException());
	next_event = 0;
	return events_nb;
}

Event Polling::getEvent(int n)
{
	if (n < 0 || n >= events_nb)
		throw (PollingException());
	return Event(events[n].data.fd, events[n].events, powerstrip[events[n].data.fd]);
}

Event Polling::nextEvent()
{
	Event ev = getEvent(next_event);
	next_event++;
	return ev;
}

TCPSocket *	Polling::getSocketByFd(int fd)
{
	return powerstrip[fd];
}

bool	Polling::isMother(Event ev) const
{
	std::list<int>::const_iterator li = std::find(mother_fds.begin(), mother_fds.end(), ev.getSocketFd());
	if (li != mother_fds.end())
		return true;
	return false;
}

//Private
Polling::Polling(Polling const & rhs)
{
	static_cast<void>(rhs);
}

void	Polling::addSocket(TCPSocket * soc, int events)
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = soc->getFd();
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, soc->getFd(), &ev))
		throw (PollingException());
	powerstrip[soc->getFd()] = soc;
}
