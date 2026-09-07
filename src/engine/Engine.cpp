#include <set>

#include "webserv/Cgi.hpp"
#include "webserv/ServerException.hpp"
#include "webserv/ServerRun.hpp"
#include "webserv/macroDef.hpp"

extern sig_atomic_t quitok;

ServerRun::ServerRun(std::vector<Server> servers) : servers(servers), status(statusCode::OK) {}

statusCode ServerRun::run() {
	try {
		loadListeningConnections();
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

void ServerRun::loadListeningConnections() {
	std::set<int> unique_ports;
	for (auto& serv : servers) {
		int port = serv.getListenPort();
		if (unique_ports.find(port) == unique_ports.end()) {
			unique_ports.insert(port);
			Connection* connection = new Connection(port);
			connection->setServers(servers);
			pool.addListeningConnection(connection);
		}
	}
}

void ServerRun::handleAllEvents(int rc) {
	for (int n = 0; n < rc; n++) {
		handleEvent();
	}
}

void ServerRun::handleEvent() {
	Event* ev = pool.nextEvent();
	ev->handleEvent();
	delete ev;
}
