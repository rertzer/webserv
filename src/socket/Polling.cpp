/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polling.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 10:06:08 by rertzer           #+#    #+#             */
/*   Updated: 2023/07/31 12:30:07 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Polling.hpp"

//Public
Polling::Polling():events_nb(0), epoll_fd(0), next_event(-1)
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
}

void	Polling::addMotherSocket(TCPSocket & soc)
{
	addSocket(soc, EPOLLIN);
}

void	Polling::addConnectSocket(TCPSocket & soc)
{
	addSocket(soc, EPOLLIN | EPOLLOUT | EPOLLET);
}

void	Polling::removeSocket(TCPSocket & soc)
{
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, soc.getFd(), NULL))
		throw (PollingException());
	powerstrip.erase(soc.getFd());
}

int	Polling::wait()
{
	events_nb = epoll_wait(epoll_fd, events, 42, -1);
	if (events_nb == -1)
		throw (PollingException());
	next_event = 0;
	return events_nb;
}
//Event(TCPSocket &, int events) 
Event Polling::getEvent(int n) const;
{
	if (n < 0 || n >= events_nb)
		throw (PollingException());
// !!!PORTEEEEEEEEEEEEEEEEE
	return Event(powerstrip[events[n].data.fd], events[n].events);
}

Event Polling::NextEvent() const;
{
	Event ev = getEvent(next_event);
	next_event++;
	return ev
}

//Private
Polling::Polling(Polling & rhs)
{
	static_cast<void>(rhs);
}

Polling	&	Polling::operator=(Polling const & rhs)
{
	static_cast<void>(rhs);
	return *this;
}

void	Polling::addSocket(TCPSocket & soc, int events)
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = soc.getFd();
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, soc.getFd(), &ev))
			throw (PollingException());
	powerstrip.insert(std::pair<int, TCPSocket &>(soc.getFd(), soc));
}
