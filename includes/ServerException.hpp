#ifndef SERVER_EXCEPTION_HPP
#define SERVER_EXCEPTION_HPP
#include <exception>

class ServerException : public std::exception {
   public:
	virtual const char* what() const throw();
};

#endif
