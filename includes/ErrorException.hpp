#ifndef ERROREXCEPTION_HPP
#define ERROREXCEPTION_HPP

#include <exception>

class ErrorException : public std::exception {
   public:
	ErrorException(int c) throw();

	int					getCode() const throw();
	virtual const char* what() const throw();

   private:
	int code;
};

#endif
