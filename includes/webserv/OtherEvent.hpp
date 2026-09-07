#ifndef INCLUDES_WEBSERV_OTHEREVENT_HPP_
#define INCLUDES_WEBSERV_OTHEREVENT_HPP_

#include <poll.h>

#include "webserv/Connection.hpp"
#include "webserv/ErrorException.hpp"
#include "webserv/Event.hpp"
#include "webserv/Request.hpp"
#include "webserv/Response.hpp"
#include "webserv/Server.hpp"
#include "webserv/macroDef.hpp"

class OtherEvent : public Event {
 public:
  OtherEvent();
  OtherEvent(OtherEvent const& rhs);
  ~OtherEvent();

  OtherEvent& operator=(Event const& rhs);

 private:
  Response getConnectionResponse() const;
  Server*  getRequestServer() const;

  bool isCgiFd() const;
  bool isCgiStatus(CgiStatus cgi_status) const;
  bool isCgiPending() const;

  void handleIn();
  void handleOut();
  void handleError();
  void handleHup();
  void handleNval();

  void handleInRequestReady();
  void handleMessageOut();

  void cgiExec();
  bool checkAndHandleCgiIn();
  void handleCgiIn();
  void handleCgiOut();

  void internalError();
  void handleErrorException(const ErrorException& e);

  void handleEventStatus();
  void handleInOk();
  void handleOutOk();
  void handleClose();
  void handleCgiPostStart();
  void handleCgiContinue();
  void handleCgiEnd();
  void handleCgiPostExec();
  void handleCgiGetExec();
  void handleCgiError();

  typedef void (OtherEvent::*handlestatus)();
};
#endif  // INCLUDES_WEBSERV_OTHEREVENT_HPP_
