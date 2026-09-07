#ifndef INCLUDES_WEBSERV_POLLING_HPP_
#define INCLUDES_WEBSERV_POLLING_HPP_

#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <list>
#include <map>

#include "webserv/Connection.hpp"
#include "webserv/Event.hpp"

class Polling {
 public:
  Polling();
  ~Polling();

  void        addListeningConnection(Connection* connection);
  void        addCgiFds(std::vector<int> fds);
  void        removeListeningConnection(int fd);
  void        removeConnection(int fd);
  void        removeCgiFd(int fd);
  int         wait();
  Event*      nextEvent();
  Connection* getConnection(nfds_t i) const;
  Connection* getConnectionByFd(int fd) const;
  Connection* getConnectionByCgiFd(int fd) const;
  void        setOut(int fd);
  void        resetOut(int fd);
  void        reset(int fd);
  void        setCgiIn(Connection* connection);
  void        addCgiFds(Connection* connection);
  void        addConnection(Connection* connection);

  class PollingException : public std::exception {
   public:
    virtual const char* what() const throw() { return ("Error: polling error"); }
  };

 private:
  Polling& operator=(const Polling& rhs);
  Polling(const Polling& rhs);
  void        addCgiFd(int fd, int events, Connection* connection);
  void        removeFd(int fd);
  Connection* getConnectionFromStrip(int fd, const std::map<int, Connection*>& strip) const;
  Event*      extractEvent(nfds_t i);

  struct pollfd              fds[256];
  nfds_t                     nfds;
  int                        events_nb;
  std::list<int>             listening_fds;
  std::map<int, Connection*> powerstrip;
  std::map<int, Connection*> powerstripCgi;
};

#endif  // INCLUDES_WEBSERV_POLLING_HPP_
