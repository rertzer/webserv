#include "webserv/ErrorException.hpp"

ErrorException::ErrorException(HttpStatus status) throw() : status(status) {}

HttpStatus ErrorException::getStatus() const throw() {
	return status;
}

const char* ErrorException::what() const throw() {
	return "Error exception";
}
