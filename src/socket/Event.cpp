#include "Event.hpp"
#include "Cgi.hpp"

Event::Event(): fd(0), events(0), status(eventStatus::NOTHING), pool(nullptr), soc(nullptr){}

Event::Event(Event const& rhs) : fd(rhs.fd), events(rhs.events), status(rhs.status), pool(rhs.pool), soc(rhs.soc) {}

Event::~Event() {}
Event& Event::operator=(Event const& rhs) {
	if (this != &rhs) {
		fd = rhs.fd;
		events = rhs.events;
		status = rhs.status;
		pool = rhs.pool;
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


void Event::setFd(int fd){
	this->fd = fd;
}

void Event::setEvents(int events){
	this->events = events;
}

void Event::setPool(Polling* pool){
	this->pool = pool;
}

void Event::setSoc(TCPSocket* soc){
	this->soc = soc;
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
void Event::handleEvent() {
	try {
		for (auto ev : poll_event) {
			handleOneEvent(ev);
			if (status != eventStatus::NOTHING) {
				handleEventStatus();
				return;
			}
		}
	} catch (const Request::RequestException& e) {
		status = eventStatus::CLOSE;
		handleEventStatus();
	} catch (const ErrorException& e) {
		status = eventStatus::IN;
		handleErrorException(e);
		handleEventStatus();
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
// Static const
int const Event::poll_event[MAX_POLL_EVENT] = {POLLERR, POLLHUP, POLLNVAL, POLLIN, POLLOUT};
