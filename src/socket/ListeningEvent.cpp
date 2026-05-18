#include "ListeningEvent.hpp"
#include "Polling.hpp"

ListeningEvent::ListeningEvent():Event(){
	
}

ListeningEvent::~ListeningEvent() {}

ListeningEvent& ListeningEvent::operator=(ListeningEvent const& rhs) {
	if (this != &rhs) {
		Event::operator=(rhs);	
	}
	return *this;
}

void ListeningEvent::handleErrorException(const ErrorException& e) {
	(void)e;
}

void ListeningEvent::handleIn() {
	TCPSocket* new_soc = soc->accept();
	if (new_soc){
		new_soc->setServers(soc->getServers());
		pool->addSocket(new_soc);
	}
}

void ListeningEvent::handleEventStatus(){}

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

void ListeningEvent::internalError(std::string message) {
		std::cerr << message << " Stopping connection on port " << soc->getPort() << std::endl;
		pool->removeSocket(fd);
}
