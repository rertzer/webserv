#include <map>

#include "webserv/HttpMethod.hpp"

std::string methodToString(HttpMethod method) {
	std::map<HttpMethod, std::string> method_str{{NONE, ""},   {GET, "GET"},	   {POST, "POST"},
												 {PUT, "PUT"}, {DELETE, "DELETE"}, {HEAD, "HEAD"}};

	return method_str[method];
}

HttpMethod stringToMethod(std::string str) {
	std::map<std::string, HttpMethod> str_method{{"", NONE},   {"GET", GET},	   {"POST", POST},
												 {"PUT", PUT}, {"DELETE", DELETE}, {"HEAD", HEAD}};

	auto it = str_method.find(str);
	if (it != str_method.end()) {
		return it->second;
	}

	return NONE;
}
