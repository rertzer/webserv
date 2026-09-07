#ifndef INCLUDES_WEBSERV_CONNECTION_HPP_
#define INCLUDES_WEBSERV_CONNECTION_HPP_
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "webserv/Request.hpp"
#include "webserv/TcpSocket.hpp"

class Connection {
 public:
  explicit Connection(int p);
  Connection(Connection const& rhs);
  Connection();
  ~Connection();
  Connection& operator=(Connection const& rhs);

  Cgi*                 getCgi() const;
  CgiStatus            getCgiStatus() const;
  int                  getPort() const;
  int                  getListeningSocketPort() const;
  int                  getSocketFd() const;
  bool                 getError() const;
  std::string          getMessageIn() const;
  std::string          getMessageOut() const;
  bool                 getKeepAlive() const;
  Server*              getDefaultServer() const;
  std::vector<Server>& getServers();
  void                 getRawData(std::string& content, int len);
  Request*             getRequest();
  std::string          getLine();

  void setMessageIn(std::string msg);
  void setMessageOut(std::string msg);
  void setError(bool er);
  void setKeepAlive(bool keep);
  void setServers(std::vector<Server> serv);

  bool isListening() const;

  void        createRequest();
  void        deleteRequest();
  void        addRawData(std::string& content, int len);
  Connection* accept() const;
  int         readAll();
  int         send();
  void        close();

 private:
  void setDefaultServer();

  int                 listening_port;
  std::string         msg_in;
  std::string         msg_out;
  bool                keep_alive;
  bool                error;
  bool                listening;
  Request*            request;
  Server*             default_server;
  std::vector<Server> servers;
  TcpSocket           soc;

  // max length to which the queue of pending connections may grow
  constexpr static size_t max_line_len = 20000;
};

#endif  // INCLUDES_WEBSERV_CONNECTION_HPP_
