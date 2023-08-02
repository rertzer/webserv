/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 13:26:24 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/02 18:10:30 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Event.hpp"

//Public
Event::Event(int sfd, int e, TCPSocket* s):soc_fd(sfd), events(e), soc(s)
{
}

Event::Event(Event const & rhs):soc_fd(rhs.soc_fd), events(rhs.events), soc(rhs.soc)
{
}

Event::~Event() {} Event & Event::operator=(Event const & rhs)
{
	if (this != &rhs)
	{
		soc_fd = rhs.soc_fd;
		events = rhs.events;
		soc = rhs.soc;
	}
	return *this;
}

int	Event::getSocketFd() const
{
	return soc_fd;
}

TCPSocket *	Event::getSocket() const
{
	return soc;
}

int	Event::getEvents() const
{
	return events;
}

bool	Event::isIn() const
{
	return (events & EPOLLIN);
}

bool	Event::isOut() const
{
	return (events & EPOLLOUT);
}

bool	Event::isRdhup() const
{
	return (events & EPOLLRDHUP);
}

bool	Event::isPri() const
{
	return (events & EPOLLPRI);
}

bool	Event::isErr() const
{
	return (events & EPOLLERR);
}

bool	Event::isHup() const
{
	return (events & EPOLLHUP);
}

bool	Event::isEt() const
{
	return (events & EPOLLET);
}

bool	Event::isOneshot() const
{
	return (events & EPOLLONESHOT);
}

void	Event::handleEvent()
{
	std::map<int, handlefun> whichfun;
	whichfun[EPOLLIN] = &Event::handleIn;
	whichfun[EPOLLOUT] = &Event::handleOut;
	whichfun[EPOLLRDHUP] = &Event::handleError;
	whichfun[EPOLLPRI] = &Event::handleError;
	whichfun[EPOLLERR] = &Event::handleError;
	whichfun[EPOLLHUP] = &Event::handleError;
	whichfun[EPOLLONESHOT] = &Event::handleError;

	for (int i = 0 ; i < 7; i++)
	{
		if (events & ev[i])
		{
			std::cout << "execute handle fun " << ev[i] << std::endl;
			handlefun fun = whichfun[ev[i]];
			(this->*fun)();
		}
	}
}

void	Event::handleIn()
{
	std::cout << "fd " << soc_fd << " is reading\n";
	soc->read();
	std::cout << soc->getMessageIn() << std::endl;
	Request req(soc_fd, soc->getMessageIn());
	soc->setMessageOut("HTTP/1.1 200 OK\r\nHost: localhost:8080\r\nConnection:close\r\n\r\nHello world!\r\n");	
}

void	Event::handleOut()
{
	if (!soc->getMessageOut().empty())
	{
		int len = soc->send();
		std::cout << "Connection fd " << soc_fd << " send " << len << " char\n";
		soc->close();
	}
	else
		std::cout << "Connection fd " << soc_fd << ": Nothing to send\n";
}

void	Event::handleError()
{
	std::cout << "Handle Error\n";
}

//Private
Event::Event()
{}

//Static const
int const    Event::ev[7] = {EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLPRI, EPOLLERR, EPOLLHUP, EPOLLONESHOT};
