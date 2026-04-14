#include "Cgi.hpp"
#include "ErrorException.hpp"
#include "Response.hpp"
#include "macroDef.hpp"
#include "utils.hpp"

void createAutoIndexResp(Request& req, Location loc, Response& rep) {
	BitSet allow_method = loc.getAllowedMethods();
	if (allow_method.getFlags() != 0) {
		rep.setAllowedMethods(allow_method);
	}

	auto method = req.getMethod();
	if ((method == GET || method == POST) && (rep.isAllowed(method))) {
		rep.setContentWithLength(rep.getDirContent(req.getQuery()));
		rep.setStatus("200 OK");
		rep.setMethod(method);
		rep.setContentType("text/html");
	} else {
		std::cerr << "fill response 88\n";
		rep.setErrorPage(405);
	}
}

void setCookie(std::string ck, Response& rep) {
	rep.getCookie().push_back(ck);
}

int initCgi(Request& req, Location& loc, Response& rep) {
	Cgi* myCgi = new Cgi(rep.getRoot(), req, loc.getExtension(), loc.getCgiPath());
	req.setCgi(myCgi);
	return 0;
}

std::pair<std::string, std::string> extractField(size_t pos, Response& rep) {
	std::string							line;
	std::pair<std::string, std::string> field;

	if (pos != std::string::npos) {
		line = rep.getContent().substr(0, pos);
		rep.getContent().erase(0, pos + 2);
		pos = line.find(":");
		if (pos != std::string::npos) {
			field.first = line.substr(0, pos);
			field.second = line.substr(pos + 1, -1);
		}
		stringTrim(field.first);
		stringTrim(field.second);
	}
	return field;
}
