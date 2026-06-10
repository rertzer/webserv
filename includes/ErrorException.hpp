#ifndef ERROREXCEPTION_HPP
#define ERROREXCEPTION_HPP

#include <exception>

#include "HttpStatus.hpp"

class ErrorException : public std::exception {
   public:
	ErrorException(HttpStatus status) throw();

	HttpStatus				getStatus() const throw();
	virtual const char* what() const throw();

   private:
	HttpStatus status;
};

#endif
