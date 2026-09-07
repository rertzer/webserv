#ifndef INCLUDES_WEBSERV_HTTPMETHOD_HPP_
#define INCLUDES_WEBSERV_HTTPMETHOD_HPP_

#include <string>

enum HttpMethod {
  NONE = 0U,
  GET = 1U << 0,
  PUT = 1U << 1,
  POST = 1U << 2,
  DELETE = 1U << 3,
  HEAD = 1U << 4,
};

std::string methodToString(HttpMethod method);
HttpMethod  stringToMethod(std::string str);

#endif  // INCLUDES_WEBSERV_HTTPMETHOD_HPP_
