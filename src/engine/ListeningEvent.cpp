#include "ListeningEvent.hpp"
#include "Polling.hpp"

/* ============================== Coplien Methods ========================== */

ListeningEvent::ListeningEvent():Event(){
	
}

ListeningEvent::ListeningEvent(Event const &rhs){
	*this = rhs;
}

ListeningEvent::~ListeningEvent() {}

ListeningEvent& ListeningEvent::operator=(ListeningEvent const& rhs) {
	if (this != &rhs) [[likely]] {
		Event::operator=(rhs);	
	}
	return *this;
}

/* =========================== handle event status ========================= */

void ListeningEvent::handleEventStatus(){}

/* =========================== Handle Poll Events ========================== */

void ListeningEvent::handleIn() {
	Connection* new_connection = connection->accept();
	if (new_connection){
		new_connection->setServers(connection->getServers());
		pool->addConnection(new_connection);
	}
}

void ListeningEvent::handleOut() {
	internalError("POLLOUT");
}

void ListeningEvent::handleError() {
	internalError("POLLERR");
}

void ListeningEvent::handleHup() {
	internalError("POLLUP");
}

void ListeningEvent::handleNval() {
	internalError("POLLNVAL");
}

/* ============================ Error Handling ============================= */

void ListeningEvent::handleErrorException(const ErrorException& e) {
	(void)e;
}

void ListeningEvent::internalError(std::string message) {
		std::cerr << message << " Stopping connection on port " << connection->getPort() << std::endl;
		pool->removeConnection(fd);
}
