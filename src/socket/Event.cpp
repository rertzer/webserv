/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 13:26:24 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/07 14:22:36 by rertzer          ###   ########.fr       */
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
	return (events & POLLIN);
}

bool	Event::isOut() const
{
	return (events & POLLOUT);
}

bool	Event::isErr() const
{
	return (events & POLLERR);
}

bool	Event::isHup() const
{
	return (events & POLLHUP);
}

int	Event::handleEvent()
{
	int	close_fd = 0;
	std::map<int, handlefun> whichfun;
	whichfun[POLLIN] = &Event::handleIn;
	whichfun[POLLOUT] = &Event::handleOut;
	whichfun[POLLERR] = &Event::handleError;
	whichfun[POLLHUP] = &Event::handleHup;
	whichfun[POLLNVAL] = &Event::handleError;

	for (int i = 0 ; i < 5; i++)
	{
		if (events & ev[i])
		{
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
	int	set_out = 0;
	try
	{
		if (soc->req == NULL)
		{
			std::cout << "Event In on fd " << soc_fd << std::endl;
			soc->req = new Request(soc, serv);
		}
		else
			soc->req->feed(serv);
		if (soc->req->ready())
		{
			Response resp(*soc->req, findTheServ(*soc->req, this->serv, soc->getMotherPort()));
			soc->setMessageOut(resp.getResponse());
			set_out = 1;
		}
	}
	catch (const Request::RequestException & e)
	{
		std::cout << e.what() << std::endl;
		set_out = soc->getFd();
	}
	catch (const ErrorException & e)
	{
		soc->setMessageOut((createErrorPage(e.getCode(), this->serv[0])).getResponse());
		soc->setKeepAlive(false);
		set_out = 1;
	}
	return set_out;
}

int	Event::handleOut()
{
	if (!soc->getMessageOut().empty())
	{
		int len = soc->send();
		std::cout << "Event Out on fd " << soc_fd << " " << len << " char sent.\n";
		if (soc->getMessageOut().empty())
		{
			delete soc->req;
			soc->req = NULL;
			if (soc->getKeepAlive())
			{
				soc->setKeepAlive(false);
				return 2;
			}
			return (soc->getFd());
		}
	}
	return 0;
}

int	Event::handleError()
{
	std::cout << "Event Error on fd " << soc_fd << std::endl;
	return (soc->getFd());
}

int	Event::handleHup()
{
	std::cout << "Event Hup on fd " << soc_fd << std::endl;
	return (soc->getFd());
}

//Private
Event::Event()
{}

//Static const
int const 	Event::ev[5] = {POLLIN, POLLOUT, POLLERR, POLLHUP, POLLNVAL};
