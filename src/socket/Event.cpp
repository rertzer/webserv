/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 13:26:24 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/09 12:00:47 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Event.hpp"


//Public
Event::Event(int sfd, int e, TCPSocket* s):soc_fd(sfd), events(e), status(0), soc(s)
{
}

Event::Event(Event const & rhs):soc_fd(rhs.soc_fd), events(rhs.events), status(rhs.status), soc(rhs.soc)
{
}

Event::~Event() {} Event & Event::operator=(Event const & rhs)
{
	if (this != &rhs)
	{
		soc_fd = rhs.soc_fd;
		events = rhs.events;
		status = rhs.status;
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

int	Event::getStatus() const
{
	return status;
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

void	Event::handleEvent()
{
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
			(this->*fun)();
			if (status) // ????????????????????????????????????????
				return;
		}
	}
}

void	Event::handleIn()
{
	try
	{
		if (status)
			handleCgiIn();
		else
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
				if (soc->req->getCgiStatus())
				{
					status = 4;
				}
				else
				{
					soc->setMessageOut(resp.getResponse());
					status = 1;
				}
			}
		}
	}
	catch (const Request::RequestException & e)
	{
		std::cout << e.what() << std::endl;
		status = 3;
	}
	catch (const ErrorException & e)
	{
		soc->setMessageOut((createErrorPage(e.getCode(), this->serv[0])).getResponse());
		soc->setKeepAlive(false);
		status = 1;
	}
}

void	Event::handleCgiIn()
{
	soc->req->getCgi().readPipeFd();
	status = 6;
}

void	Event::handleOut()
{
	if (status)
	{
		handleCgiOut();
	}
	else
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
					status =  2;
				}
				status = 3;
			}
		}
	}
}

void	Event::handleCgiOut()
{

	soc->req->getCgi().writePostFd();
	status = 7;

}

void	Event::handleError()
{
	std::cout << "Event Error on fd " << soc_fd << std::endl;
	status = 3;
}

void	Event::handleHup()
{
	std::cout << "Event Hup on fd " << soc_fd << std::endl;
	status = 3;
}

//Private
Event::Event()
{}

//Static const
int const 	Event::ev[5] = {POLLIN, POLLOUT, POLLERR, POLLHUP, POLLNVAL};
