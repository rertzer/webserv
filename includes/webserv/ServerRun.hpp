#ifndef INCLUDES_WEBSERV_SERVERRUN_HPP_
#define INCLUDES_WEBSERV_SERVERRUN_HPP_

#include <vector>

#include "webserv/Polling.hpp"
#include "webserv/Server.hpp"

class ServerRun {
 public:
  explicit ServerRun(std::vector<Server> servers);
  statusCode run();

 private:
  void handleAllEvents(int rc);
  void handleEvent();
  void loadListeningConnections();

  std::vector<Server> servers;
  Polling             pool;
  statusCode          status;
};

#endif  // INCLUDES_WEBSERV_SERVERRUN_HPP_
