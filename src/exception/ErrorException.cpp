#include "ErrorException.hpp"

ErrorException::ErrorException(int c) throw() : code(c) {}

int ErrorException::getCode() const throw() {
	return code;
}

const char* ErrorException::what() const throw() {
	return "Error exception";
}
