/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverRun.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:30:59 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/17 14:28:06 by rertzer          ###   ########.fr       */
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

		while (1)
		{
			int rc = pool.wait();
			if (quitok)
				break;

			for (int n = 0; n < rc; n++)
				handleEvent(pool, serv);
		}
	}
	catch (const TCPSocket::SocketException & e)
	{
		std::cerr << e.what() << std::endl;
		return 2;
	}
	catch (const Cgi::CgiException & e)
	{
		std::cerr << e.what() << std::endl;
		return 3;
	}
	catch (const Polling::PollingException & e)
	{
		std::cerr << e.what() << std::endl;
		return 4;
	}
	catch (const ServerException & e)
	{
		std::cerr << e.what() << std::endl;
		return 5;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 6;
	}
	std::cout << GREEN "Good bye!" << WHITE << std::endl;
	return 0;
}

void	loadMotherSocket(Polling & pool, std::vector<Server> serv)
{
	std::map<int, int>	unique_port;
	for (std::vector<Server>::iterator it = serv.begin(); it != serv.end(); it++)
	{
		int vp = it->getListenPort();
		unique_port[vp] = 1;
	}
	for (std::map<int, int>::iterator it = unique_port.begin(); it != unique_port.end(); it++)
		pool.addMotherSocket(it->first);
}

void	handleEvent(Polling & pool, std::vector<Server> & serv)
{
	Event 	ev = pool.nextEvent();
	ev.setServ(serv);

	if (pool.isMother(ev))
		eventOnMother(ev, pool);
	else
		eventOnOther(ev, pool);
	pool.reset(ev.getFd());
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
		std::cout << event_msg << ". Stopping connection on port " << port << std::endl;
		pool.removeSocket(ev.getFd());
	}
}

void	eventOnOther(Event & ev, Polling & pool)
{
	ev.handleEvent();
	if (ev.getStatus())
		handleEventStatus(ev, pool);
}

void	handleEventStatus(Event & ev, Polling & pool)
{
	std::map<int, handlestatus>	whichandle;
	whichandle[1] = &handleInOk;
	whichandle[2] = &handleOutOk;
	whichandle[3] = &handleClose;
	whichandle[4] = &handleCgiPostStart;
	whichandle[5] = &handleCgiContinue;
	whichandle[6] = &handleCgiEnd;
	whichandle[7] = &handleCgiPostExec;
	whichandle[8] = &handleCgiGetExec;
	whichandle[9] = &handleCgiError;

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

void	handleCgiPostStart(Event & ev, Polling & pool)
{
	pool.addCgiFds(ev.getSocket());
	ev.cgiExec();
}

void	handleCgiContinue(Event & ev, Polling & pool)
{
	(void)pool;
	(void)ev;
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
}

void	handleCgiGetExec(Event & ev, Polling & pool)
{
	pool.addCgiFds(ev.getSocket());
	ev.cgiExec();
}

void	handleCgiError(Event & ev, Polling & pool)
{
	std::cout << RED "Cgi Error. Stopping connection.\n";
	pool.removeCgiFd(ev.getSocket()->req->getCgi()->getFds()[2]);
	pool.removeSocket(ev.getFd());
}
