#ifndef INCLUDES_WEBSERV_ERROREXCEPTION_HPP_
#define INCLUDES_WEBSERV_ERROREXCEPTION_HPP_
#include <exception>

#include "webserv/HttpStatus.hpp"

class ErrorException : public std::exception {
 public:
  explicit ErrorException(HttpStatus status) throw();

  HttpStatus          getStatus() const throw();
  virtual const char* what() const throw();

 private:
  HttpStatus status;
};

#endif  // INCLUDES_WEBSERV_ERROREXCEPTION_HPP_
