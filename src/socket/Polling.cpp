/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polling.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 10:06:08 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/07 11:42:00 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Polling.hpp"

extern sig_atomic_t	quitok;

//Public
Polling::Polling():nfds(0), events_nb(0)
{
	memset(fds, 0, sizeof(fds));
}

Polling::~Polling()
{
	for (std::map<int, TCPSocket*>::iterator it = powerstrip.begin(); it != powerstrip.end(); it++)
	{
		delete (it->second);
	}
}

Polling	&	Polling::operator=(Polling const & rhs)
{
	if (this != &rhs)
	{
		memset(fds, 0, sizeof(fds));
		for (i = 0; i < rhs.nfds; i++)
			fds[i] = rhs.fds[i];
		nfds = rhs.nfds;
		events_nb = rhs.events_nb;
		mother_fds = rhs.mother_fds;
		powerstrip = rhs.powerstrip;
	}
	return *this;
}

void	Polling::addMotherSocket(int port)
{
	TCPSocket *	soc = new TCPSocket(port);
	addSocket(soc);
	mother_fds.push_back(soc->getFd());
}

void	Polling::connect(Event const & ev)
{
	TCPSocket *	soc = new TCPSocket();
	powerstrip[ev.getSocketFd()]->accept(soc);
	addSocket(soc);

	std::cout << "New connection fd: " << soc->getFd() << std::endl;
}

void	Polling::removeMotherSocket(int fd)
{
	mother_fds.remove(fd);
	removeSocket(fd);
}

void	Polling::removeSocket(int fd)
{
	std::cout << "Removing socket fd " << fd << std::endl;
	int	i = 0;
	for (; i < nfds; i++)
	{
		if (fds[i].fd == fd)
		{
			nfds--;
			break;
		}
	}
	if (i < nfds)
	{
		fds[i] = fds[nfds];
	}
	fds[nfds].fd = 0;
	fds[nfsd].events = 0;
	fds[nfds].revents = 0;

	delete (powerstrip[fd]);
	powerstrip.erase(fd);
}

int	Polling::wait()
{
	events_nb = ::poll(fds, nfds, -1);
	if (quitok)
		return 0;
	if (events_nb == -1)
		throw (PollingException());
	next_event = 0;
	return events_nb;
}

Event Polling::getEvent(int n)
{
	if (n < 0 || n >= events_nb)
		throw (PollingException());

	return Event(events[n].data.fd, events[n].events, powerstrip[events[n].data.fd]);
}

Event Polling::nextEvent()
{
	if (!events_nb)
		throw (PollingException());

	for (int i = 0; i < nfds; i++)
	{
		if (fds[i] != 0)
		{
			events_nb--;
			short rev = fds[i].revents;
			fds[i].revents = 0;
			return Event(fds[i].fd, rev, powerstrip[fds[i].fd]);
		}
	}
	throw (PollingException());
}

TCPSocket *	Polling::getSocketByFd(int fd)
{
	return powerstrip[fd];
}

bool	Polling::isMother(Event ev) const
{
	std::list<int>::const_iterator li = std::find(mother_fds.begin(), mother_fds.end(), ev.getSocketFd());
	if (li != mother_fds.end())
		return true;
	return false;
}

void	Polling::setOut(int fd)
{
	for (int i = 0; i < nfds; i++)
	{
		if (fds[i].fd == fd)
		{
			fds[i].events = POLLIN | POLLOUT;
			break;
		}
	}
}

void	Polling::resetOut(int fd)
{
	for (int i = 0; i < nfds; i++)
	{
		if (fds[i].fd == fd)
		{
			fds[i].events = POLLIN;
			break;
		}
	}
}

//Private
Polling::Polling(Polling const & rhs)
{
	static_cast<void>(rhs);
}

void	Polling::addSocket(TCPSocket * soc)
{
	if (nfds > 254)
		throw(PollingException());
	fds[nfds].fd = soc->getFd();
	fds[nfds].events = POLLIN;
	nfds++;

	powerstrip[soc->getFd()] = soc;
}
