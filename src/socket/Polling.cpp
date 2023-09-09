/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polling.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/31 10:06:08 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/09 12:00:58 by rertzer          ###   ########.fr       */
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
		for (nfds_t i = 0; i < rhs.nfds; i++)
			fds[i] = rhs.fds[i];
		nfds = rhs.nfds;
		events_nb = rhs.events_nb;
		mother_fds = rhs.mother_fds;
		powerstrip = rhs.powerstrip;
		powerstripCgi = rhs.powerstripCgi;
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
	nfds_t	i = 0;
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
	fds[nfds].events = 0;
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
	return events_nb;
}

Event Polling::nextEvent()
{
	if (!events_nb)
		throw (PollingException());

	for (nfds_t i = 0; i < nfds; i++)
	{
		if (fds[i].revents != 0)
		{
			events_nb--;
			short rev = fds[i].revents;
			fds[i].revents = 0;
			int	status = 0;
			TCPSocket * soc = getSocketByFd(fds[i].fd);
			if (soc == NULL)
			{
				soc = getSocketByCgiFd(fds[i].fd);
				status = 4;
			}
			Event ev(fds[i].fd, rev, soc);
			ev.setStatus(status);
			return ev;
		}
	}
	throw (PollingException());
}

TCPSocket *	Polling::getSocketByFd(int fd)
{
	std::map<int, TCPSocket *>::const_iterator mi = powerstrip.find(fd);
	if (mi != powerstrip.end())
		return *mi;
	return NULL;
}

TCPSocket * Polling::getSocketByCgiFd(int fd)
{
	std::map<int, TCPSocket *>::const_iterator mi = powerstripCgi.find(fd);
	if (mi != powerstripCgi.end())
		return *mi;
	return NULL;
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
	for (nfds_t i = 0; i < nfds; i++)
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
	for (nfds_t i = 0; i < nfds; i++)
	{
		if (fds[i].fd == fd)
		{
			fds[i].events = POLLIN;
			break;
		}
	}
}

void	Polling::reset(int fd)
{
	for (nfds_t i = 0; i < nfds; i++)
	{
		if (fds[i].fd == fd)
		{
			fds[i].revents = 0;
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
