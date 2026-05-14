#include <set>

#include "Cgi.hpp"
#include "ServerException.hpp"
#include "color.hpp"
#include "macroDef.hpp"
#include "ServerRun.hpp"


extern sig_atomic_t quitok;

ServerRun::ServerRun(std::vector<Server> servers):servers(servers), status(statusCode::OK){
	
}

statusCode ServerRun::run() {
	try {
		loadListeningSocket();
		std::cout << "Listening...\n";

		while (1) {
			int rc = pool.wait();
			if (quitok) {
				break;
			}

			for (int n = 0; n < rc; n++) {
				handleEvent();
			}
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

void ServerRun::loadListeningSocket() {
	std::set<int> unique_ports;
	for (auto& serv : servers) {
		int port = serv.getListenPort();
		if (unique_ports.find(port) == unique_ports.end()) {
			unique_ports.insert(port);
			TCPSocket* soc = new TCPSocket(port);
			soc->setServers(servers);
			pool.addListeningSocket(soc);
		}
	}
}

void ServerRun::handleEvent() {
	Event ev = pool.nextEvent();

	if (ev.getSocket()->getListening()) {
		eventOnListeningSocket(ev);
	} else {
		eventOnOther(ev);
	}
	pool.reset(ev.getFd());
}

void ServerRun::eventOnListeningSocket(Event& ev) {
	if (ev.isIn()) {
		TCPSocket* soc = ev.getSocket()->accept();
		soc->setServers(servers);
		pool.addSocket(soc);
	}
	checkBadEventOnListeningSocket(ev);
}

void ServerRun::checkBadEventOnListeningSocket(Event& ev) {
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

void ServerRun::eventOnOther(Event& ev) {
	ev.handleEvent();

	if (ev.getStatus() != eventStatus::NOTHING) {
		handleEventStatus(ev);
	}
}

void ServerRun::handleEventStatus(Event& ev) {
	std::map<eventStatus, handlestatus> handlers;
	handlers[eventStatus::IN] = &ServerRun::handleInOk;
	handlers[eventStatus::OUT] = &ServerRun::handleOutOk;
	handlers[eventStatus::CLOSE] = &ServerRun::handleClose;
	handlers[eventStatus::CGI_INIT] = &ServerRun::handleCgiPostStart;
	handlers[eventStatus::CGI_CONTINUE] = &ServerRun::handleCgiContinue;
	handlers[eventStatus::CGI_CLOSE] = &ServerRun::handleCgiEnd;
	handlers[eventStatus::CGI_POST_EXEC] = &ServerRun::handleCgiPostExec;
	handlers[eventStatus::CGI_GET_EXEC] = &ServerRun::handleCgiGetExec;
	handlers[eventStatus::CGI_ERROR] = &ServerRun::handleCgiError;

	handlestatus hs = handlers[ev.getStatus()];
	(this->*hs)(ev);
}

void ServerRun::handleInOk(Event& ev) {
	pool.setOut(ev.getFd());
}

void ServerRun::handleOutOk(Event& ev) {
	pool.resetOut(ev.getFd());
}

void ServerRun::handleClose(Event& ev) {
	pool.removeSocket(ev.getFd());
}

void ServerRun::handleCgiPostStart(Event& ev) {
	pool.addCgiFds(ev.getSocket());
	ev.cgiExec();
}

void ServerRun::handleCgiContinue(Event& ev) {
	(void)pool;
	(void)ev;
}

void ServerRun::handleCgiEnd(Event& ev) {
	pool.removeCgiFd(ev.getFd());
	pool.setOut(ev.getSocket()->getFd());
}

void ServerRun::handleCgiPostExec(Event& ev) {
	pool.setCgiIn(ev.getSocket());
	pool.removeCgiFd(ev.getFd());
}

void ServerRun::handleCgiGetExec(Event& ev) {
	pool.addCgiFds(ev.getSocket());
	ev.cgiExec();
}

void ServerRun::handleCgiError(Event& ev) {
	std::cerr << RED "Cgi Error. Stopping connection.\n";
	pool.removeCgiFd(ev.getSocket()->req->getCgi()->getFds()[2]);
	pool.removeSocket(ev.getFd());
}
