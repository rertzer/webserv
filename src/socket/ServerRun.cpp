#include <set>

#include "Cgi.hpp"
#include "ServerException.hpp"
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
			handleAllEvents(rc);	
		}
	} catch (const TcpSocket::SocketException& e) {
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
			Connection* soc = new Connection(port);
			soc->setServers(servers);
			pool.addListeningSocket(soc);
		}
	}
}

void ServerRun::handleAllEvents(int rc){
	for (int n = 0; n < rc; n++) {
		handleEvent();
	}
}

void ServerRun::handleEvent() {
	Event* ev = pool.nextEvent();
	ev->handleEvent();
	delete ev;
}
