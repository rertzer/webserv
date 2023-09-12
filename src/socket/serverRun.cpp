/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverRun.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:30:59 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/12 13:08:46 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serverRun.hpp"

extern sig_atomic_t	quitok;

int	serverRun(std::vector<Server> serv)
{
	try
	{
		Polling pool;
		loadMotherSocket(pool, serv);
		std::cout << "Listening...\n";
		int counter = 0;
		
		while (1)
		{
			int rc = pool.wait();
			counter++;
			std::cout << "loop " << counter << std::endl;
			if (quitok)
				break;

			for (int n = 0; n < rc; n++)
				handleEvent(pool, serv);
		}
	}
	catch (const TCPSocket::SocketException & e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch (const Polling::PollingException & e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

void	handleEvent(Polling & pool, std::vector<Server> & serv)
{
	Event 	ev = pool.nextEvent();
	ev.setServ(serv);

	if (pool.isMother(ev))
		eventOnMother(ev, pool);
	else
		eventOnOther(ev, pool);			
	std::cout << "Resetting " << ev.getFd() << std::endl;
	pool.reset(ev.getFd());
}

void	loadMotherSocket(Polling & pool, std::vector<Server> serv)
{
	std::map<int, int>	unique_port;
	for (std::vector<Server>::iterator it = serv.begin(); it != serv.end(); it++)
	{
		std::vector<int>	vp = it->getListenPort();
		for (std::vector<int>::iterator jt = vp.begin(); jt != vp.end(); jt++)
			unique_port[*jt] = 1;
	}
	for (std::map<int, int>::iterator it = unique_port.begin(); it != unique_port.end(); it++)
		pool.addMotherSocket(it->first);
}

void	eventOnMother(Event & ev, Polling & pool) 
{
	if (ev.isIn())
		pool.connect(ev);
	checkBadEventOnMother(ev, pool);
}

void	checkBadEventOnMother(Event & ev, Polling & pool)
{
	std::string	event_msg;
	if (ev.isErr())
		event_msg += "EPOLLERR ";
	if (ev.isHup())
		event_msg += "EPOLLHUP ";
	if (!event_msg.empty())
	{
		int port = ev.getSocket()->getPort();
		std::cout << event_msg << ". Restarting connection on port " << port << std::endl;
		pool.removeSocket(ev.getFd());
		pool.addMotherSocket(port);
	}
}

void	eventOnOther(Event & ev, Polling & pool)
{
	std::cout << "EVENT on " << ev.getFd() << " socket " << ev.getSocket()->getFd() << std::endl;
	ev.handleEvent();
	handleEventStatus(ev, pool);
}

void	handleEventStatus(Event & ev, Polling & pool)
{
	std::map<int, handlestatus>	whichandle;
	whichandle[1] = &handleInOk;
	whichandle[2] = &handleOutOk;
	whichandle[3] = &handleClose;
	whichandle[4] = &handleCgiStart;
	whichandle[6] = &handleCgiEnd;
	whichandle[7] = &handleCgiPostExec;
	whichandle[8] = &handleCgiGetExec;
	whichandle[9] = &handleCgiError;

	std::cout << "event status is " << ev.getStatus() << std::endl;
	handlestatus hs = whichandle[ev.getStatus()];
	(hs)(ev, pool);
}

void	handleInOk(Event & ev, Polling & pool)
{
	pool.setOut(ev.getFd());
}

void	handleOutOk(Event & ev, Polling & pool)
{
	pool.resetOut(ev.getFd());
}

void	handleClose(Event & ev, Polling & pool)
{
	pool.removeSocket(ev.getFd());
}

void	handleCgiStart(Event & ev, Polling & pool)
{
	pool.addCgiFds(ev.getSocket());
}

void	handleCgiEnd(Event & ev, Polling & pool)
{
	pool.removeCgiFd(ev.getFd());
	pool.setOut(ev.getSocket()->getFd());
}

void	handleCgiPostExec(Event & ev, Polling & pool)
{
	pool.setCgiIn(ev.getSocket());
	pool.removeCgiFd(ev.getFd());
	ev.cgiExec();
}

void	handleCgiGetExec(Event & ev, Polling & pool)
{
	pool.addCgiFds(ev.getSocket());
	ev.cgiExec();
}

void	handleCgiError(Event & ev, Polling & pool)
{
	pool.removeCgiFd(ev.getFd());
}
