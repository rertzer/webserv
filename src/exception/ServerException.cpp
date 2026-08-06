#include "ServerException.hpp"

const char* ServerException::what() const throw() {
	return "Error: Server parsing error";
}
