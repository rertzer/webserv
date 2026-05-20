#include "OtherEvent.hpp"
#include "Cgi.hpp"
#include "Polling.hpp"
#include "color.hpp"

/* ================================ Coplien Methods ======================== */

OtherEvent::OtherEvent(): Event(){}


OtherEvent::OtherEvent(OtherEvent const &rhs){
	*this = rhs;
}

OtherEvent::~OtherEvent() {}

OtherEvent& OtherEvent::operator=(Event const& rhs) {
	if (this != &rhs) {
		Event::operator=(rhs);	
	}
	return *this;
}

/* =================================== Getters ============================= */
Response OtherEvent::getSocketResponse() const {
	Response resp(*soc->req);
	return resp;
}

Server* OtherEvent::getRequestServer() const {
	return soc->req->getServer();
}

/* ============================== Is Methods =============================== */

bool OtherEvent::isCgiFd() const {
	if (fd == getSocket()->getFd())
		return false;
	return true;
}

bool OtherEvent::isCgiStatus(CgiStatus cgi_status) const {
	return (soc->req->getCgiStatus() == cgi_status);
}

bool OtherEvent::isCgiPending() const {
	if (soc->req && soc->req->getCgi() && soc->req->getCgi()->getPid()) {
		return true;
	}
	return false;
}

/* ============================== Handle Poll Methods =========================== */

void OtherEvent::handleIn() {
	if (soc->getError()) {
		soc->readAll();
		return;
	}
	if (soc->req == nullptr) {
		soc->req = new Request(soc);
	} else if (checkAndHandleCgiIn()) {
		return;
	}
	soc->req->printCleanRequest();
	if (soc->req->ready()) {
		handleInRequestReady();
	}
}

void OtherEvent::handleOut() {
	if (soc->req &&
		(isCgiStatus(CgiStatus::WAIT_WRITE_POST) || isCgiStatus(CgiStatus::POST_TO_READ))) {
		handleCgiOut();
	} else if (!soc->getMessageOut().empty()) {
		handleMessageOut();
	}
}

void OtherEvent::handleError() {
	internalError();
}

void OtherEvent::handleHup() {
	status = eventStatus::CLOSE;
	if (isCgiFd()) {
		soc->req->getCgi()->closePipe();
		Response resp = getSocketResponse();
		soc->setMessageOut(resp.getResponse());
		status = eventStatus::CGI_CLOSE;
	} else if (isCgiPending()) {
		soc->req->getCgi()->stop();
	}
}

void OtherEvent::handleNval() {
	status = eventStatus::CLOSE;
}

/* =============================== Handle In and Out ======================= */

void OtherEvent::handleInRequestReady() {
	Response resp = getSocketResponse();
	if (isCgiStatus(CgiStatus::WAIT_WRITE_POST) || isCgiStatus(CgiStatus::READY_EXEC)) {
		status = eventStatus::CGI_INIT;
	} else {
		soc->setMessageOut(resp.getResponse());
		status = eventStatus::IN;
	}
}

void OtherEvent::handleMessageOut() {
	soc->send();
	if (soc->getMessageOut().empty()) {
		soc->deleteRequest();
	}
	if (soc->getKeepAlive()) {
		soc->setKeepAlive(false);
		status = eventStatus::OUT;
	} else {
		status = eventStatus::CLOSE;
	}
}

/* ============================ CGI ======================================== */

void OtherEvent::cgiExec() {
	soc->req->getCgi()->exec();
}

bool OtherEvent::checkAndHandleCgiIn() {
	if (isCgiStatus(CgiStatus::WAIT_READ_PIPE)) {
		handleCgiIn();
		return true;
	} else if (isCgiStatus(CgiStatus::NO_INIT)) {
		soc->req->feed();
	}
	return false;
}

void OtherEvent::handleCgiIn() {
	if (!isCgiFd()) {
		soc->req->getCgi()->closePipe();
		status = eventStatus::CGI_ERROR;
		return;
	} else {
		soc->req->getCgi()->readPipeFd();
	}

	if (isCgiStatus(CgiStatus::DONE)) {
		Response resp = getSocketResponse();
		soc->setMessageOut(resp.getResponse());
		status = eventStatus::CGI_CLOSE;
	} else {
		status = eventStatus::NOTHING;
	}
}

void OtherEvent::handleCgiOut() {
	soc->req->getCgi()->writePostFd();
	if (isCgiStatus(CgiStatus::WAIT_READ_PIPE))
		status = eventStatus::CGI_POST_EXEC;
	else
		status = eventStatus::CGI_CONTINUE;
}

/* =============================== Handle Exception ======================== */

void OtherEvent::internalError() {
	if (isCgiFd()) {
		status = eventStatus::CGI_CLOSE;
		throw(ErrorException(500));
	} else {
		status = eventStatus::CLOSE;
	}
}

void OtherEvent::handleErrorException(const ErrorException& e) {
	Server server;
	if (soc->req == nullptr) {
		server = *soc->getDefaultServer();
	} else {
		if (!isCgiStatus(CgiStatus::NO_INIT)) {
			status = eventStatus::CGI_INIT;
		}
		server = *getRequestServer();
	}
	soc->setMessageOut((Response(server, e.getCode())).getResponse());
	soc->setKeepAlive(false);
	soc->setError(true);
	if (status == eventStatus::NOTHING) {
		status = eventStatus::IN;
	}
}

/* ============================= Handle Event Status ======================= */

void OtherEvent::handleEventStatus() {
	std::map<eventStatus, handlestatus> handlers;
	handlers[eventStatus::IN] = &OtherEvent::handleInOk;
	handlers[eventStatus::OUT] = &OtherEvent::handleOutOk;
	handlers[eventStatus::CLOSE] = &OtherEvent::handleClose;
	handlers[eventStatus::CGI_INIT] = &OtherEvent::handleCgiPostStart;
	handlers[eventStatus::CGI_CONTINUE] = &OtherEvent::handleCgiContinue;
	handlers[eventStatus::CGI_CLOSE] = &OtherEvent::handleCgiEnd;
	handlers[eventStatus::CGI_POST_EXEC] = &OtherEvent::handleCgiPostExec;
	handlers[eventStatus::CGI_GET_EXEC] = &OtherEvent::handleCgiGetExec;
	handlers[eventStatus::CGI_ERROR] = &OtherEvent::handleCgiError;
	handlestatus hs = handlers[status];
	(this->*hs)();
}

void OtherEvent::handleInOk() {
	pool->setOut(fd);
}

void OtherEvent::handleOutOk() {
	pool->resetOut(fd);
}

void OtherEvent::handleClose() {
	pool->removeSocket(fd);
}

void OtherEvent::handleCgiPostStart() {
	pool->addCgiFds(soc);
	cgiExec();
}

void OtherEvent::handleCgiContinue() {
}

void OtherEvent::handleCgiEnd() {
	pool->removeCgiFd(fd);
	pool->setOut(soc->getFd());
}

void OtherEvent::handleCgiPostExec() {
	pool->setCgiIn(soc);
	pool->removeCgiFd(fd);
}

void OtherEvent::handleCgiGetExec() {
	pool->addCgiFds(soc);
	cgiExec();
}

void OtherEvent::handleCgiError() {
	std::cerr << RED "Cgi Error. Stopping connection.\n";
	pool->removeCgiFd(soc->req->getCgi()->getFds()[2]);
	pool->removeSocket(fd);
}
