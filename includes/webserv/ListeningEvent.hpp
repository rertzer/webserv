#ifndef INCLUDES_WEBSERV_LISTENINGEVENT_HPP_
#define INCLUDES_WEBSERV_LISTENINGEVENT_HPP_
#include <poll.h>
#include <string>

#include "webserv/Event.hpp"

class Polling;

class ListeningEvent : public Event {
 public:
  ListeningEvent();
  explicit ListeningEvent(ListeningEvent const& rhs);
  ~ListeningEvent();

  ListeningEvent& operator=(ListeningEvent const& rhs);
  void            handleEvent();

 private:
  void handleIn();
  void handleOut();
  void handleError();
  void handleHup();
  void handleNval();
  void internalError(std::string message);
  void handleErrorException(const ErrorException& e);
  void handleEventStatus();
};
#endif  // INCLUDES_WEBSERV_LISTENINGEVENT_HPP_
