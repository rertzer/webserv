/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 13:26:24 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/02 12:13:27 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Event.hpp"

//Public
Event::Event(int sfd, int e):soc_fd(sfd), events(e)
{
}

Event::Event(Event const & rhs):soc_fd(rhs.soc_fd), events(rhs.events)
{
}

Event::~Event() {} Event & Event::operator=(Event const & rhs)
{
	if (this != &rhs)
	{
		soc_fd = rhs.soc_fd;
		events = rhs.events;
	}
	return *this;
}

int	Event::getSocketFd() const
{
	return soc_fd;
}

int	Event::getEvents() const
{
	return events;
}

bool	Event::isIn()
{
	return (events & EPOLLIN);
}

bool	Event::isOut()
{
	return (events & EPOLLOUT);
}

bool	Event::isRdhup()
{
	return (events & EPOLLRDHUP);
}

bool	Event::isPri()
{
	return (events & EPOLLPRI);
}

bool	Event::isErr()
{
	return (events & EPOLLERR);
}

bool	Event::isHup()
{
	return (events & EPOLLHUP);
}

bool	Event::isEt()
{
	return (events & EPOLLET);
}

bool	Event::isOneshot()
{
	return (events & EPOLLONESHOT);
}

void	handleEvent()
{
	std::map<int, handlefun> whichfun;
	whichfun[EPOLLIN] = &handleIn;
	whichfun[EPOLLOUT] = &handleOut;
	whichfun[EPOLLRDHUP] = &handleError;
	whichfun[EPOLLPRI] = &handleError;
	whichfun[EPOLLERR] = &handleError;
	whichfun[EPOLLHUP] = &handleError;
	whichfun[EPOLLONESHOT] = &handlError;

	for (std::vector<int>::iterator it = ev.begin; it != ev.end; it++)
	{
		if (events & *it)
			whichfun[*it]();
	}
	std::vector<handlefun> = {&handleIn, &handleOut, &handleError};

}

//Private
Event::Event()
{}

//Static const
std::vector<int>    ev = {EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLPRI, EPOLLERR, EPOLLHUP, EPOLLONESHOT};

