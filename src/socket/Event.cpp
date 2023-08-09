/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 13:26:24 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/09 13:20:29 by rertzer          ###   ########.fr       */
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
			close_fd = (this->*fun)();
			std::cout << "return value is " << close_fd << std::endl;
			if (close_fd)
				return close_fd;
		}
	}
	return close_fd;
}

int	Event::handleIn()
{
	int	line_nb = 0;
	try
	{
		std::cout << "fd " << soc_fd << " is reading\n";
		std::string line = soc->readLine();
		std::cout << "line is: $" << line << "$\n";
		std::cout << "remain: $" << soc->getMessageIn() << "$" << std::endl;
		Request req(soc->getPort(), line);
		while (line.length())
		{
			if (line_nb >= line_max)
				break;
			line = soc->readLine();
			if (line.length())
			{
				req.addField(line);
				line_nb++;
			}
		}
		std::string h = req.getField("Host");
		std::cout << "Host is : $" << h << "$" << std::endl;
		std::cout << "Accept-Encoding values:\n";
		h = req.getField("Accept-Encoding");
		std::vector<std::string> acc = splitCsv(h);
		for (unsigned int i = 0; i < acc.size(); i++)
			std::cout << acc[i] << std::endl;
		soc->setMessageOut("HTTP/1.1 200 OK\r\nHost: localhost:8080\r\nConnection:close\r\n\r\nHello world!\r\n");
	}
	catch (const ErrorException & e)
	{
		std::stringstream ss;
		ss << "HTTP/1.1 " << Status::getMsg(e.getCode()) << "\r\nHost: localhost:8080\r\nConnection:close\r\n\r\nSomething Bad Happened\r\n";
		std::cerr << e.what() << " " << Status::getMsg(e.getCode()) << std::endl;
		soc->setMessageOut(ss.str());
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
	std::cout << "Connection fd " << soc_fd << ": Nothing to send\n";
	return 0;
}

int	Event::handleError()
{
	std::cout << "Handle Error\n";
	return 0;
}

//Private
Event::Event()
{}

//Static const
int const 	Event::ev[7] = {EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLPRI, EPOLLERR, EPOLLHUP, EPOLLONESHOT};
int const	Event::line_max = 1024;
