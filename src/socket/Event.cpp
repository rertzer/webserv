/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 13:26:24 by rertzer           #+#    #+#             */
/*   Updated: 2023/07/31 17:34:45 by rertzer          ###   ########.fr       */
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

Event::~Event()
{}

Event & Event::operator=(Event const & rhs)
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

int	Event::getEvents()
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

//Private
Event::Event()
{}
