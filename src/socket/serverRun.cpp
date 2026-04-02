#include <set>

#include "Cgi.hpp"
#include "ServerException.hpp"
#include "color.hpp"
#include "macroDef.hpp"
#include "serverRun.hpp"

extern sig_atomic_t quitok;

statusCode serverRun(std::vector<Server> serv) {
	statusCode status = statusCode::OK;
	try {
		Polling pool;
		loadListeningSocket(pool, serv);
		std::cout << "Listening...\n";

		while (1) {
			int rc = pool.wait();
			if (quitok)
				break;

			for (int n = 0; n < rc; n++)
				handleEvent(pool, serv);
		}
	} catch (const TCPSocket::SocketException& e) {
		status = handleException(e, statusCode::SOCKET);
	} catch (const Cgi::CgiException& e) {
		status = handleException(e, statusCode::CGI);
	} catch (const Polling::PollingException& e) {
		status = handleException(e, statusCode::POLLING);
	} catch (const ServerException& e) {
		status = handleException(e, statusCode::SERVER);
	} catch (const std::exception& e) {
		status = handleException(e, statusCode::STANDARD);
	}
	return status;
}

void loadListeningSocket(Polling& pool, std::vector<Server> servers) {
	std::set<int> unique_ports;
	for (auto& serv : servers) {
		int port = serv.getListenPort();
		if (unique_ports.find(port) == unique_ports.end()) {
			unique_ports.insert(port);
			pool.addListeningSocket(port);
		}
	}
}

void handleEvent(Polling& pool, std::vector<Server>& serv) {
	Event ev = pool.nextEvent();
	ev.setServ(serv);

	if (pool.isListeningSocket(ev)) {
		eventOnListeningSocket(ev, pool);
	} else {
		eventOnOther(ev, pool);
	}
	pool.reset(ev.getFd());
}

void eventOnListeningSocket(Event& ev, Polling& pool) {
	if (ev.isIn()) {
		pool.connect(ev);
	}
	checkBadEventOnListeningSocket(ev, pool);
}

void checkBadEventOnListeningSocket(Event& ev, Polling& pool) {
	std::string event_msg;
	if (ev.isErr()) {
		event_msg += "EPOLLERR ";
	}
	if (ev.isHup()) {
		event_msg += "EPOLLHUP ";
	}
	if (!event_msg.empty()) {
		int port = ev.getSocket()->getPort();
		std::cout << event_msg << ". Stopping connection on port " << port << std::endl;
		pool.removeSocket(ev.getFd());
	}
}

void eventOnOther(Event& ev, Polling& pool) {
	ev.handleEvent();
	if (ev.getStatus() != eventStatus::NOTHING) {
		handleEventStatus(ev, pool);
	}
}

void handleEventStatus(Event& ev, Polling& pool) {
	std::map<int, handlestatus> whichandle;
	whichandle[1] = &handleInOk;
	whichandle[2] = &handleOutOk;
	whichandle[3] = &handleClose;
	whichandle[4] = &handleCgiPostStart;
	whichandle[5] = &handleCgiContinue;
	whichandle[6] = &handleCgiEnd;
	whichandle[7] = &handleCgiPostExec;
	whichandle[8] = &handleCgiGetExec;
	whichandle[9] = &handleCgiError;

	handlestatus hs = whichandle[static_cast<int>(ev.getStatus())];
	(hs)(ev, pool);
}

void handleInOk(Event& ev, Polling& pool) {
	pool.setOut(ev.getFd());
}

void handleOutOk(Event& ev, Polling& pool) {
	pool.resetOut(ev.getFd());
}

void handleClose(Event& ev, Polling& pool) {
	pool.removeSocket(ev.getFd());
}

void handleCgiPostStart(Event& ev, Polling& pool) {
	pool.addCgiFds(ev.getSocket());
	ev.cgiExec();
}

void handleCgiContinue(Event& ev, Polling& pool) {
	(void)pool;
	(void)ev;
}

void handleCgiEnd(Event& ev, Polling& pool) {
	pool.removeCgiFd(ev.getFd());
	pool.setOut(ev.getSocket()->getFd());
}

void handleCgiPostExec(Event& ev, Polling& pool) {
	pool.setCgiIn(ev.getSocket());
	pool.removeCgiFd(ev.getFd());
}

void handleCgiGetExec(Event& ev, Polling& pool) {
	pool.addCgiFds(ev.getSocket());
	ev.cgiExec();
}

void handleCgiError(Event& ev, Polling& pool) {
	std::cout << RED "Cgi Error. Stopping connection.\n";
	pool.removeCgiFd(ev.getSocket()->req->getCgi()->getFds()[2]);
	pool.removeSocket(ev.getFd());
}
