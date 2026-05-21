#include "ListeningEvent.hpp"
#include "Polling.hpp"

/* ============================== Coplien Methods ========================== */

ListeningEvent::ListeningEvent():Event(){
	
}

ListeningEvent::~ListeningEvent() {}

ListeningEvent& ListeningEvent::operator=(ListeningEvent const& rhs) {
	if (this != &rhs) {
		Event::operator=(rhs);	
	}
	return *this;
}

/* =========================== handle event status ========================= */

void ListeningEvent::handleEventStatus(){}

/* =========================== Handle Poll Events ========================== */

void ListeningEvent::handleIn() {
	Connection* new_soc = soc->accept();
	if (new_soc){
		new_soc->setServers(soc->getServers());
		pool->addSocket(new_soc);
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
		std::cerr << message << " Stopping connection on port " << soc->getPort() << std::endl;
		pool->removeSocket(fd);
}
