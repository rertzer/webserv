/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 13:26:24 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/23 09:40:27 by rertzer          ###   ########.fr       */
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

void	Event::setServ(std::vector<Server> s)
{
	serv = s;
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

int	Event::handleEvent()
{
	int	close_fd = 0;
	std::map<int, handlefun> whichfun;
	whichfun[EPOLLIN] = &Event::handleIn;
	whichfun[EPOLLOUT] = &Event::handleOut;
	whichfun[EPOLLERR] = &Event::handleError;
	whichfun[EPOLLHUP] = &Event::handleError;

	for (int i = 0 ; i < 4; i++)
	{
		if (events & ev[i])
		{
			//std::cout << "execute handle fun " << ev[i] << std::endl;
			handlefun fun = whichfun[ev[i]];
			close_fd = (this->*fun)();
			if (close_fd)
				return close_fd;
		}
	}
	return close_fd;
}

int	Event::handleIn()
{
	try
	{
		Request req(soc);
		try
		{
			Response resp(req, findTheServ(req, this->serv, soc->getMotherPort()));
			std::cout << resp.getResponse() << std::endl;
			soc->setMessageOut(resp.getResponse());
		}
		catch (const ErrorException & e)
		{
			soc->setMessageOut((createErrorPage(e.getCode(), this->serv[0])).getResponse());
		}
	}
	catch(const ErrorException & e)
	{
		soc->setMessageOut((createErrorPage(e.getCode(), this->serv[0])).getResponse());
	}
	return 0;
}

int	Event::handleOut()
{
	if (!soc->getMessageOut().empty())
	{
		int len = soc->send();
		std::cout << "Connection fd " << soc_fd << " send " << len << " char\n";
		return (soc->getFd());
	}
	//std::cout << "Connection fd " << soc_fd << ": Nothing to send\n";
	return 0;
}

int	Event::handleError()
{
	std::cout << "Handle Error\n";
	return (soc->getFd());
}

//Private
Event::Event()
{}

//Static const
int const 	Event::ev[4] = {EPOLLIN, EPOLLOUT, EPOLLERR, EPOLLHUP};
