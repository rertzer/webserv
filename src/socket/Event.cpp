#include "Event.hpp"
#include "Cgi.hpp"
#include "ErrorException.hpp"

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
	serv = s;
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
	std::map<int, handlefun> whichfun;
	whichfun[POLLIN] = &Event::handleIn;
	whichfun[POLLOUT] = &Event::handleOut;
	whichfun[POLLERR] = &Event::handleError;
	whichfun[POLLHUP] = &Event::handleHup;
	whichfun[POLLNVAL] = &Event::handleNval;

	try {
		for (int i = 0; i < 5; i++) {
			if (events & ev[i]) {
				handlefun fun = whichfun[ev[i]];
				(this->*fun)();
				if (status != eventStatus::NOTHING)
					return;
			}
		}
	} catch (const Request::RequestException& e) {
		status = eventStatus::CLOSE;

	} catch (const ErrorException& e) {
		if (soc->req == NULL) {
			soc->setMessageOut(
				(createErrorPage(e.getCode(),
								 findTheDefaultServ(serv, soc->getListeningSocketPort()))
					 .getResponse()));
		} else {
			if (soc->req->getCgiStatus())
				status = eventStatus::CGI_INIT;
			soc->setMessageOut(
				(createErrorPage(e.getCode(),
								 findTheServ(*soc->req, serv, soc->getListeningSocketPort())))
					.getResponse());
		}
		soc->setKeepAlive(false);
		soc->setError(true);
		if (status == eventStatus::NOTHING)
			status = eventStatus::IN;
	}
}

void Event::handleIn() {
	if (soc->getError()) {
		soc->readAll();
		return;
	}
	if (soc->req == NULL) {
		soc->req = new Request(soc, serv);
	} else {
		if (soc->req->getCgiStatus() == 3) {
			handleCgiIn();
			return;
		} else if (soc->req->getCgiStatus() == 0) {
			soc->req->feed(serv);
		}
	}
	printCleanRequest(*soc->req);
	if (soc->req->ready()) {
		Response resp(*soc->req, findTheServ(*soc->req, this->serv, soc->getListeningSocketPort()));
		if (soc->req->getCgiStatus() == 1)
			status = eventStatus::CGI_INIT;
		else if (soc->req->getCgiStatus() == 2)
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
	if (soc->req->getCgiStatus() == 4) {
		Response resp(*soc->req, findTheServ(*soc->req, this->serv, soc->getListeningSocketPort()));
		soc->setMessageOut(resp.getResponse());
		status = eventStatus::CGI_CLOSE;
	} else
		status = eventStatus::NOTHING;
}

void Event::handleOut() {
	if (soc->req && (soc->req->getCgiStatus() == 1 || soc->req->getCgiStatus() == 5))
		handleCgiOut();
	else {
		if (!soc->getMessageOut().empty()) {
			soc->send();
			if (soc->getMessageOut().empty()) {
				if (soc->req != NULL) {
					delete soc->req;
					soc->req = NULL;
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
	if (soc->req->getCgiStatus() == 3)
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
		Response resp(*soc->req, findTheServ(*soc->req, this->serv, soc->getListeningSocketPort()));
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
int const Event::ev[5] = {POLLERR, POLLHUP, POLLNVAL, POLLIN, POLLOUT};
