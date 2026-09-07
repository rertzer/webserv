#ifndef INCLUDES_WEBSERV_SERVEREXCEPTION_HPP_
#define INCLUDES_WEBSERV_SERVEREXCEPTION_HPP_
#include <exception>

class ServerException : public std::exception {
 public:
  virtual const char* what() const throw();
};

#endif  // INCLUDES_WEBSERV_SERVEREXCEPTION_HPP_
