#include <cstring>

#include "webserv/Cgi.hpp"
#include "webserv/ListeningEvent.hpp"
#include "webserv/OtherEvent.hpp"
#include "webserv/Polling.hpp"

extern sig_atomic_t quitok;

Polling::Polling() : nfds(0), events_nb(0) {
	memset(fds, 0, sizeof(fds));
}

Polling::~Polling() {
	for (auto it = powerstrip.begin(); it != powerstrip.end(); it++) {
		delete (it->second);
	}
}

Polling& Polling::operator=(Polling const& rhs) {
	if (this != &rhs) {
		memset(fds, 0, sizeof(fds));
		for (nfds_t i = 0; i < rhs.nfds; i++) {
			fds[i] = rhs.fds[i];
		}
		nfds = rhs.nfds;
		events_nb = rhs.events_nb;
		listening_fds = rhs.listening_fds;
		powerstrip = rhs.powerstrip;
		powerstripCgi = rhs.powerstripCgi;
	}
	return *this;
}

void Polling::addListeningConnection(Connection* connection) {
	addConnection(connection);
	listening_fds.push_back(connection->getSocketFd());
}

void Polling::removeListeningConnection(int fd) {
	listening_fds.remove(fd);
	removeConnection(fd);
}

void Polling::removeConnection(int fd) {
	auto it = powerstrip.find(fd);
	if (it != powerstrip.end()) {
		removeFd(fd);
		delete (powerstrip[fd]);
		powerstrip.erase(fd);
	}
}

void Polling::removeCgiFd(int fd) {
	auto it = powerstripCgi.find(fd);
	if (it != powerstripCgi.end()) {
		removeFd(fd);
		powerstripCgi.erase(fd);
	}
}

int Polling::wait() {
	events_nb = ::poll(fds, nfds, -1);
	if (quitok) {
		return 0;
	}
	if (events_nb == -1)
		throw(PollingException());
	return events_nb;
}

Event* Polling::nextEvent() {
	if (!events_nb)
		throw(PollingException());

	for (nfds_t i = 0; i < nfds; i++) {
		if (fds[i].revents != 0) {
			return extractEvent(i);
		}
	}
	throw(PollingException());
}

Connection* Polling::getConnection(nfds_t i) const {
	Connection* connection = getConnectionByFd(fds[i].fd);
	if (!connection) {
		connection = getConnectionByCgiFd(fds[i].fd);
		if (!connection) {
			std::cerr << "Connection not Found\n";
			throw(PollingException());
		}
	}
	return connection;
}

Connection* Polling::getConnectionByFd(int fd) const {
	return getConnectionFromStrip(fd, powerstrip);
}

Connection* Polling::getConnectionByCgiFd(int fd) const {
	return getConnectionFromStrip(fd, powerstripCgi);
}

Connection* Polling::getConnectionFromStrip(int fd, const std::map<int, Connection*>& strip) const {
	auto it = strip.find(fd);
	if (it != strip.end()) {
		return it->second;
	}
	return nullptr;
}

void Polling::setOut(int fd) {
	for (nfds_t i = 0; i < nfds; i++) {
		if (fds[i].fd == fd) {
			fds[i].events = POLLIN | POLLOUT;
			break;
		}
	}
}

void Polling::resetOut(int fd) {
	for (nfds_t i = 0; i < nfds; i++) {
		if (fds[i].fd == fd) {
			fds[i].events = POLLIN;
			break;
		}
	}
}

void Polling::reset(int fd) {
	for (nfds_t i = 0; i < nfds; i++) {
		if (fds[i].fd == fd) {
			fds[i].revents = 0;
			break;
		}
	}
}
void Polling::setCgiIn(Connection* connection) {
	auto cgi_fds = connection->getCgi()->getFds();
	for (nfds_t i = 0; i < nfds; i++) {
		if (fds[i].fd == cgi_fds[2]) {
			fds[i].events = POLLIN;
			break;
		}
	}
}

void Polling::addCgiFds(Connection* connection) {
	std::vector<int> cgi_fds = connection->getCgi()->getFds();
	if (cgi_fds[1] != -1) {
		addCgiFd(cgi_fds[1], POLLOUT, connection);
		addCgiFd(cgi_fds[2], 0, connection);
	} else if (cgi_fds[2] != -1)
		addCgiFd(cgi_fds[2], POLLIN, connection);
}

// Private
Polling::Polling(Polling const& rhs) {
	static_cast<void>(rhs);
}

void Polling::addConnection(Connection* connection) {
	if (nfds > 254)
		throw(PollingException());
	fds[nfds].fd = connection->getSocketFd();
	fds[nfds].events = POLLIN;
	nfds++;

	powerstrip[connection->getSocketFd()] = connection;
}

void Polling::addCgiFd(int fd, int events, Connection* connection) {
	if (nfds > 254)
		throw(PollingException());
	fds[nfds].fd = fd;
	fds[nfds].events = events;
	nfds++;
	powerstripCgi[fd] = connection;
}

void Polling::removeFd(int fd) {
	nfds_t i = 0;
	for (; i < nfds; i++) {
		if (fds[i].fd == fd) {
			nfds--;
			break;
		}
	}
	if (i < nfds) {
		fds[i] = fds[nfds];
	}
	fds[nfds].fd = 0;
	fds[nfds].events = 0;
	fds[nfds].revents = 0;
}

Event* Polling::extractEvent(nfds_t i) {
	Event* ev = nullptr;

	events_nb--;
	short rev = fds[i].revents;
	fds[i].revents = 0;
	Connection* connection = getConnection(i);
	if (connection->isListening()) {
		ev = new ListeningEvent();
	} else {
		ev = new OtherEvent();
	}
	ev->setFd(fds[i].fd);
	ev->setEvents(rev);
	ev->setPool(this);
	ev->setConnection(connection);
	return ev;
}
