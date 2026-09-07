#include <iostream>
#include <map>

#include "webserv/Cgi.hpp"
#include "webserv/OtherEvent.hpp"
#include "webserv/Polling.hpp"
#include "webserv/color.hpp"

/* ================================ Coplien Methods ======================== */

OtherEvent::OtherEvent() : Event() {}

OtherEvent::OtherEvent(OtherEvent const& rhs) {
  *this = rhs;
}

OtherEvent::~OtherEvent() {}

OtherEvent& OtherEvent::operator=(Event const& rhs) {
  if (this != &rhs) [[likely]] {
    Event::operator=(rhs);
  }
  return *this;
}

/* =================================== Getters ============================= */
Response OtherEvent::getConnectionResponse() const {
  Response resp(*connection->getRequest());
  return resp;
}

Server* OtherEvent::getRequestServer() const {
  return connection->getRequest()->getServer();
}

/* ============================== Is Methods =============================== */

bool OtherEvent::isCgiFd() const {
  return fd != getConnection()->getSocketFd();
}

bool OtherEvent::isCgiStatus(CgiStatus cgi_status) const {
  return connection->getCgiStatus() == cgi_status;
}

bool OtherEvent::isCgiPending() const {
  return connection->getCgiStatus() != CgiStatus::NO_INIT;
}

/* ============================== Handle Poll Methods =========================== */

void OtherEvent::handleIn() {
  if (connection->getError()) {
    connection->readAll();
    return;
  }
  if (connection->getRequest() == nullptr) {
    connection->createRequest();
  } else if (checkAndHandleCgiIn()) {
    return;
  }
  connection->getRequest()->printCleanRequest();
  if (connection->getRequest()->ready()) {
    handleInRequestReady();
  }
}

void OtherEvent::handleOut() {
  if (connection->getRequest() &&
      (isCgiStatus(CgiStatus::WAIT_WRITE_POST) || isCgiStatus(CgiStatus::POST_TO_READ))) {
    handleCgiOut();
  } else if (!connection->getMessageOut().empty()) {
    handleMessageOut();
  }
}

void OtherEvent::handleError() {
  internalError();
}

void OtherEvent::handleHup() {
  status = eventStatus::CLOSE;
  if (isCgiFd()) {
    connection->getCgi()->closePipe();
    Response resp = getConnectionResponse();
    connection->setMessageOut(resp.getResponse());
    status = eventStatus::CGI_CLOSE;
  } else if (isCgiPending()) {
    connection->getCgi()->stop();
  }
}

void OtherEvent::handleNval() {
  status = eventStatus::CLOSE;
}

/* =============================== Handle In and Out ======================= */

void OtherEvent::handleInRequestReady() {
  Response resp = getConnectionResponse();
  if (isCgiStatus(CgiStatus::WAIT_WRITE_POST) || isCgiStatus(CgiStatus::READY_EXEC)) {
    status = eventStatus::CGI_INIT;
  } else {
    connection->setMessageOut(resp.getResponse());
    status = eventStatus::IN;
  }
}

void OtherEvent::handleMessageOut() {
  connection->send();
  if (connection->getMessageOut().empty()) {
    connection->deleteRequest();
  }
  if (connection->getKeepAlive()) {
    connection->setKeepAlive(false);
    status = eventStatus::OUT;
  } else {
    status = eventStatus::CLOSE;
  }
}

/* ============================ CGI ======================================== */

void OtherEvent::cgiExec() {
  connection->getCgi()->exec();
}

bool OtherEvent::checkAndHandleCgiIn() {
  if (isCgiStatus(CgiStatus::WAIT_READ_PIPE)) {
    handleCgiIn();
    return true;
  } else if (isCgiStatus(CgiStatus::NO_INIT)) {
    connection->getRequest()->feed();
  }
  return false;
}

void OtherEvent::handleCgiIn() {
  if (!isCgiFd()) {
    connection->getCgi()->closePipe();
    status = eventStatus::CGI_ERROR;
    return;
  } else {
    connection->getCgi()->readPipeFd();
  }

  if (isCgiStatus(CgiStatus::DONE)) {
    Response resp = getConnectionResponse();
    connection->setMessageOut(resp.getResponse());
    status = eventStatus::CGI_CLOSE;
  } else {
    status = eventStatus::NOTHING;
  }
}

void OtherEvent::handleCgiOut() {
  connection->getCgi()->writePostFd();
  if (isCgiStatus(CgiStatus::WAIT_READ_PIPE)) {
    status = eventStatus::CGI_POST_EXEC;
  } else {
    status = eventStatus::CGI_CONTINUE;
  }
}

/* =============================== Handle Exception ======================== */

void OtherEvent::internalError() {
  if (isCgiFd()) {
    status = eventStatus::CGI_CLOSE;
    throw(ErrorException(HttpStatus::INTERNAL_SERVER_ERROR));
  } else {
    status = eventStatus::CLOSE;
  }
}

void OtherEvent::handleErrorException(const ErrorException& e) {
  Server server;
  if (connection->getRequest() == nullptr) {
    server = *connection->getDefaultServer();
  } else {
    if (!isCgiStatus(CgiStatus::NO_INIT)) {
      status = eventStatus::CGI_INIT;
    }
    server = *getRequestServer();
  }
  connection->setMessageOut((Response(server, e.getStatus())).getResponse());
  connection->setKeepAlive(false);
  connection->setError(true);
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
  pool->removeConnection(fd);
}

void OtherEvent::handleCgiPostStart() {
  pool->addCgiFds(connection);
  cgiExec();
}

void OtherEvent::handleCgiContinue() {}

void OtherEvent::handleCgiEnd() {
  pool->removeCgiFd(fd);
  pool->setOut(connection->getSocketFd());
}

void OtherEvent::handleCgiPostExec() {
  pool->setCgiIn(connection);
  pool->removeCgiFd(fd);
}

void OtherEvent::handleCgiGetExec() {
  pool->addCgiFds(connection);
  cgiExec();
}

void OtherEvent::handleCgiError() {
  std::cerr << RED "Cgi Error. Stopping connection.\n";
  pool->removeCgiFd(connection->getCgi()->getFds()[2]);
  pool->removeConnection(fd);
}
