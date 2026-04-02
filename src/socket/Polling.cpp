#include "Polling.hpp"
#include "Cgi.hpp"

#include <algorithm>

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
		for (nfds_t i = 0; i < rhs.nfds; i++)
			fds[i] = rhs.fds[i];
		nfds = rhs.nfds;
		events_nb = rhs.events_nb;
		listening_fds = rhs.listening_fds;
		powerstrip = rhs.powerstrip;
		powerstripCgi = rhs.powerstripCgi;
	}
	return *this;
}

void Polling::addListeningSocket(int port) {
	TCPSocket* soc = new TCPSocket(port);
	addSocket(soc);
	listening_fds.push_back(soc->getFd());
}

void Polling::connect(Event const& ev) {
	TCPSocket* soc = new TCPSocket();
	powerstrip[ev.getFd()]->accept(soc);
	addSocket(soc);
}

void Polling::removeListeningSocket(int fd) {
	listening_fds.remove(fd);
	removeSocket(fd);
}

void Polling::removeSocket(int fd) {
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

Event Polling::nextEvent() {
	if (!events_nb)
		throw(PollingException());

	for (nfds_t i = 0; i < nfds; i++) {
		if (fds[i].revents != 0) {
			return extractEvent(i);
		}
	}
	throw(PollingException());
}

TCPSocket* Polling::getSocketByFd(int fd) {
	return getSocketFromStrip(fd, powerstrip);
}

TCPSocket* Polling::getSocketByCgiFd(int fd) {
	return getSocketFromStrip(fd, powerstripCgi);
}

bool Polling::isListeningSocket(Event ev) const {
	auto li = std::find(listening_fds.begin(), listening_fds.end(), ev.getFd());
	if (li != listening_fds.end())
		return true;
	return false;
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
void Polling::setCgiIn(TCPSocket* soc) {
	auto cgi_fds = soc->req->getCgi()->getFds();
	for (nfds_t i = 0; i < nfds; i++) {
		if (fds[i].fd == cgi_fds[2]) {
			fds[i].events = POLLIN;
			break;
		}
	}
}

void Polling::addCgiFds(TCPSocket* soc) {
	std::vector<int> cgi_fds = soc->req->getCgi()->getFds();
	if (cgi_fds[1] != -1) {
		addCgiFd(cgi_fds[1], POLLOUT, soc);
		addCgiFd(cgi_fds[2], 0, soc);
	} else if (cgi_fds[2] != -1)
		addCgiFd(cgi_fds[2], POLLIN, soc);
}

// Private
Polling::Polling(Polling const& rhs) {
	static_cast<void>(rhs);
}

void Polling::addSocket(TCPSocket* soc) {
	if (nfds > 254)
		throw(PollingException());
	fds[nfds].fd = soc->getFd();
	fds[nfds].events = POLLIN;
	nfds++;

	powerstrip[soc->getFd()] = soc;
}

void Polling::addCgiFd(int fd, int events, TCPSocket* soc) {
	if (nfds > 254)
		throw(PollingException());
	fds[nfds].fd = fd;
	fds[nfds].events = events;
	nfds++;
	powerstripCgi[fd] = soc;
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

TCPSocket* Polling::getSocketFromStrip(int fd, std::map<int, TCPSocket*>& strip) const {
	auto it = strip.find(fd);
	if (it != strip.end())
		return it->second;
	return nullptr;
}

Event Polling::extractEvent(nfds_t i) {
	events_nb--;
	short rev = fds[i].revents;
	fds[i].revents = 0;
	TCPSocket* soc = getSocketByFd(fds[i].fd);
	if (soc == nullptr)
		soc = getSocketByCgiFd(fds[i].fd);
	return Event(fds[i].fd, rev, soc);
}
