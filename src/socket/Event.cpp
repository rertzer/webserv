/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 13:26:24 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/16 13:56:03 by rertzer          ###   ########.fr       */
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
		status = 3;

	}
	catch (const ErrorException & e)
	{
		if (soc->req == NULL)
		{
			soc->setMessageOut((createErrorPage(e.getCode(), findTheDefaultServ(serv, soc->getMotherPort())).getResponse()));
		}
		else
		{
			soc->setMessageOut((createErrorPage(e.getCode(), findTheServ(*soc->req,serv, soc->getMotherPort()))).getResponse());
		}
		soc->setKeepAlive(false);
		soc->setError(true);
		if (! status)
			status = 1;
	}
}

void	Event::handleIn()
{
	if (soc->getError())
	{
		soc->readAll();
		return;
	}
	if (soc->req == NULL)
	{
		soc->req = new Request(soc, serv);
	}
	else
	{
		if (soc->req->getCgiStatus() == 3)
		{
			handleCgiIn();
			return;
		}
		else if (soc->req->getCgiStatus() == 0)
			soc->req->feed(serv);
	}
	printCleanRequest(*soc->req);
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
		Response resp(*soc->req, findTheServ(*soc->req, this->serv, soc->getMotherPort()));
		soc->setMessageOut(resp.getResponse());
		status = 6;
	}
	else
		status = 0;
}

void	Event::handleOut()
{
	if (soc->req && (soc->req->getCgiStatus() == 1 || soc->req->getCgiStatus() == 5))
		handleCgiOut();
	else
	{
		if (!soc->getMessageOut().empty())
		{
			soc->send();
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
	internalError();
}

void	Event::handleHup()
{
	if (isCgiFd())
	{
		soc->req->getCgi()->closePipe();
		Response resp(*soc->req, findTheServ(*soc->req, this->serv, soc->getMotherPort()));
		soc->setMessageOut(resp.getResponse());
		status = 6;
	}
	else
		status = 3;
}

void	Event::handleNval()
{
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
int const 	Event::ev[5] = {POLLERR, POLLHUP, POLLNVAL, POLLIN, POLLOUT};
