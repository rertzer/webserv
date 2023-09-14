/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 13:26:24 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/14 11:00:30 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Event.hpp"


//Public
Event::Event(int f, int e, TCPSocket* s):fd(f), events(e), status(0), soc(s)
{
}

Event::Event(Event const & rhs):fd(rhs.fd), events(rhs.events), status(rhs.status), soc(rhs.soc)
{
}

Event::~Event() {} Event & Event::operator=(Event const & rhs)
{
	if (this != &rhs)
	{
		fd = rhs.fd;
		events = rhs.events;
		status = rhs.status;
		soc = rhs.soc;
	}
	return *this;
}

int	Event::getFd() const
{
	return fd;
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

bool	Event::isCgiFd() const
{
	if (fd == getSocket()->getFd())
		return false;
	return true;
}

void	Event::handleEvent()
{
	std::map<int, handlefun> whichfun;
	whichfun[POLLIN] = &Event::handleIn;
	whichfun[POLLOUT] = &Event::handleOut;
	whichfun[POLLERR] = &Event::handleError;
	whichfun[POLLHUP] = &Event::handleHup;
	whichfun[POLLNVAL] = &Event::handleNval;


	try
	{
		for (int i = 0 ; i < 5; i++)
		{
			if (events & ev[i])
			{
				handlefun fun = whichfun[ev[i]];
				(this->*fun)();
				if (status)
					return;
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
			soc->setMessageOut((createErrorPage(e.getCode(), findTheServ(*soc->req,serv, soc->req->getPort()))).getResponse());
			soc->setKeepAlive(false);
			if (! status)
				status = 1;
	}
}

void	Event::handleIn()
{
		std::cout << "Event In on fd " << fd << std::endl;
		if (soc->req == NULL)
		{
			soc->req = new Request(soc, serv);
		}
		else
		{
			if (soc->req->getCgiStatus() == 3)
			{
				std::cout << "Handling CgiIn()\n";
				handleCgiIn();
				return;
			}
			else if (soc->req->getCgiStatus() == 0)
				soc->req->feed(serv);
		}
		if (soc->req->ready())
		{
			Response resp(*soc->req, findTheServ(*soc->req, this->serv, soc->getMotherPort()));
			if (soc->req->getCgiStatus() == 1)
				status = 4;
			else if (soc->req->getCgiStatus() == 2)
				status = 4;
			else
			{
				soc->setMessageOut(resp.getResponse());
				status = 1;
			}
		}

}

void	Event::handleCgiIn()
{
	soc->req->getCgi()->readPipeFd();
	if (soc->req->getCgiStatus() == 4)
	{
		std::cout << "query asked = " << soc->req->getQuery() << std::endl;
		Response resp(*soc->req, findTheServ(*soc->req, this->serv, soc->getMotherPort()));
		soc->setMessageOut(resp.getResponse());
		status = 6;
	}
	else
		status = 0;
}

void	Event::handleOut()
{

	std::cout << "Handle out request is " << soc->req << std::endl;
	if (soc->req && (soc->req->getCgiStatus() == 1 || soc->req->getCgiStatus() == 5))
		handleCgiOut();
	else
	{
		if (!soc->getMessageOut().empty())
		{
			int len = soc->send();
			std::cout << "Event Out on fd " << fd << " " << len << " char sent.\n";
			if (soc->getMessageOut().empty())
			{
				if (soc->req != NULL)
				{
					delete soc->req;
					soc->req = NULL;
				}
				if (soc->getKeepAlive())
				{
					soc->setKeepAlive(false);
					status =  2;
				}
				else
					status = 3;
			}
		}
	}
}

void	Event::handleCgiOut()
{
	soc->req->getCgi()->writePostFd();
	if (soc->req->getCgiStatus() == 3)
		status = 7;
	else
		status = 5;
}

void	Event::handleError()
{
	std::cout << "Event Error on fd " << fd << std::endl;
	internalError();
}

void	Event::handleHup()
{
	std::cout << "Event Hup on fd " << fd << std::endl;
	if (isCgiFd())
	{
		status = 6;
		throw (ErrorException(500));
	}
	else
		status = 3;
}

void	Event::handleNval()
{
	std::cout << "Event Nval on fd " << fd << std::endl;
	internalError();
}

void	Event::internalError()
{
	if (isCgiFd())
	{
		status = 6;
		throw (ErrorException(500));
	}
	else
		status = 3;
}

void	Event::cgiExec()
{
	soc->req->getCgi()->exec();
}

//Private
Event::Event()
{}

//Static const
int const 	Event::ev[5] = {POLLIN, POLLOUT, POLLERR, POLLHUP, POLLNVAL};
