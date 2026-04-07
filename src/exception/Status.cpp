#include <map>
#include <sstream>

#include "Status.hpp"

std::string Status::getReason(int code) {
	std::map<int, std::string> reason{
		{100, "Continue"},
		{200, "OK"},
		{201, "Created"},
		{202, "Accepted"},
		{204, "No Content"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{303, "See Other"},
		{304, "Not Modified"},
		{307, "Temporary Redirect"},
		{308, "Permanent Redirect"},
		{400, "Bad Request"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{406, "Not Acceptable"},
		{408, "Request Timeout"},
		{417, "Expectation Failed"},
		{418, "I'm a teapot"},
		{451, "Unavailable For Legal Reasons"},
		{500, "Internal Server Error"},
		{501, "Not Implemented"},
		{502, "Bad Gateway"},
		{503, "Service Unavailable"},
		{504, "Gateway Timeout"},
		{505, "HTTP Version Not Supported"},
	};

	std::map<int, std::string>::const_iterator cit = reason.find(code);
	if (cit == reason.end())
		return "";
	return cit->second;
}

std::string Status::getMsg(int code) {
	std::stringstream ss;
	ss << code << " " << getReason(code);
	return ss.str();
}
