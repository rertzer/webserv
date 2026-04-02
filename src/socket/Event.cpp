#include "Event.hpp"
#include "Cgi.hpp"

// Public
Event::Event(int f, int e, TCPSocket* s) : fd(f), events(e), status(eventStatus::NOTHING), soc(s) {}

Event::Event(Event const& rhs) : fd(rhs.fd), events(rhs.events), status(rhs.status), soc(rhs.soc) {}

Event::~Event() {}
Event& Event::operator=(Event const& rhs) {
	if (this != &rhs) {
		fd = rhs.fd;
		events = rhs.events;
		status = rhs.status;
		soc = rhs.soc;
	}
	return *this;
}

int Event::getFd() const {
	return fd;
}

TCPSocket* Event::getSocket() const {
	return soc;
}

int Event::getEvents() const {
	return events;
}

eventStatus Event::getStatus() const {
	return status;
}

void Event::setServ(std::vector<Server> s) {
	servers = s;
}

bool Event::isIn() const {
	return (events & POLLIN);
}

bool Event::isOut() const {
	return (events & POLLOUT);
}

bool Event::isErr() const {
	return (events & POLLERR);
}

bool Event::isHup() const {
	return (events & POLLHUP);
}

bool Event::isCgiFd() const {
	if (fd == getSocket()->getFd())
		return false;
	return true;
}

void Event::handleEvent() {
	try {
		for (auto ev : poll_event) {
			handleOneEvent(ev);
			if (status != eventStatus::NOTHING) {
				return;
			}
		}
	} catch (const Request::RequestException& e) {
		status = eventStatus::CLOSE;
	} catch (const ErrorException& e) {
		handleErrorException(e);
	}
}

void Event::handleOneEvent(int ev) {
	static std::map<int, handlefun> whichfun{{POLLIN, &Event::handleIn},
											 {POLLOUT, &Event::handleOut},
											 {POLLERR, &Event::handleError},
											 {POLLHUP, &Event::handleHup},
											 {POLLNVAL, &Event::handleNval}};

	if (events & ev) {
		handlefun fun = whichfun[ev];
		(this->*fun)();
	}
}

void Event::handleErrorException(const ErrorException& e) {
	Server server;
	if (soc->req == nullptr) {
		server = findTheDefaultServ(servers, soc->getListeningSocketPort());
	} else {
		if (soc->req->getCgiStatus() != CgiStatus::NO_INIT) {
			status = eventStatus::CGI_INIT;
		}
		server = findTheServ(*soc->req, servers, soc->getListeningSocketPort());
	}
	soc->setMessageOut((createErrorPage(e.getCode(), server)).getResponse());
	soc->setKeepAlive(false);
	soc->setError(true);
	if (status == eventStatus::NOTHING) {
		status = eventStatus::IN;
	}
}

void Event::handleIn() {
	if (soc->getError()) {
		soc->readAll();
		return;
	}
	if (soc->req == nullptr) {
		soc->req = new Request(soc, servers);
	} else {
		if (soc->req->getCgiStatus() == CgiStatus::WAIT_READ_PIPE) {
			handleCgiIn();
			return;
		} else if (soc->req->getCgiStatus() == CgiStatus::NO_INIT) {
			soc->req->feed(servers);
		}
	}
	printCleanRequest(*soc->req);
	if (soc->req->ready()) {
		Response resp(*soc->req, findTheServ(*soc->req, servers, soc->getListeningSocketPort()));
		if (soc->req->getCgiStatus() == CgiStatus::WAIT_WRITE_POST)
			status = eventStatus::CGI_INIT;
		else if (soc->req->getCgiStatus() == CgiStatus::READY_EXEC)
			status = eventStatus::CGI_INIT;
		else {
			soc->setMessageOut(resp.getResponse());
			status = eventStatus::IN;
		}
	}
}

void Event::handleCgiIn() {
	if (!isCgiFd()) {
		soc->req->getCgi()->closePipe();
		status = eventStatus::CGI_ERROR;
		return;
	} else
		soc->req->getCgi()->readPipeFd();
	if (soc->req->getCgiStatus() == CgiStatus::DONE) {
		Response resp(*soc->req, findTheServ(*soc->req, servers, soc->getListeningSocketPort()));
		soc->setMessageOut(resp.getResponse());
		status = eventStatus::CGI_CLOSE;
	} else
		status = eventStatus::NOTHING;
}

void Event::handleOut() {
	if (soc->req && (soc->req->getCgiStatus() == CgiStatus::WAIT_WRITE_POST ||
					 soc->req->getCgiStatus() == CgiStatus::POST_TO_READ))
		handleCgiOut();
	else {
		if (!soc->getMessageOut().empty()) {
			soc->send();
			if (soc->getMessageOut().empty()) {
				if (soc->req != nullptr) {
					delete soc->req;
					soc->req = nullptr;
				}
				if (soc->getKeepAlive()) {
					soc->setKeepAlive(false);
					status = eventStatus::OUT;
				} else
					status = eventStatus::CLOSE;
			}
		}
	}
}

void Event::handleCgiOut() {
	soc->req->getCgi()->writePostFd();
	if (soc->req->getCgiStatus() == CgiStatus::WAIT_READ_PIPE)
		status = eventStatus::CGI_POST_EXEC;
	else
		status = eventStatus::CGI_CONTINUE;
}

void Event::handleError() {
	internalError();
}

void Event::handleHup() {
	status = eventStatus::CLOSE;
	if (isCgiFd()) {
		soc->req->getCgi()->closePipe();
		Response resp(*soc->req, findTheServ(*soc->req, servers, soc->getListeningSocketPort()));
		soc->setMessageOut(resp.getResponse());
		status = eventStatus::CGI_CLOSE;
	} else if (cgiIsPending())
		soc->req->getCgi()->stop();
}

void Event::handleNval() {
	status = eventStatus::CLOSE;
}

bool Event::cgiIsPending() {
	if (soc->req && soc->req->getCgi() && soc->req->getCgi()->getPid())
		return true;
	return false;
}

void Event::internalError() {
	if (isCgiFd()) {
		status = eventStatus::CGI_CLOSE;
		throw(ErrorException(500));
	} else
		status = eventStatus::CLOSE;
}

void Event::cgiExec() {
	soc->req->getCgi()->exec();
}

// Private
Event::Event() {}

// Static const
int const Event::poll_event[MAX_POLL_EVENT] = {POLLERR, POLLHUP, POLLNVAL, POLLIN, POLLOUT};
