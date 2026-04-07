#include "Response.hpp"
#include "macroDef.hpp"

void dealWithGet(Request req, Response& rep) {
	rep.setMethod(GET);
	fillPart(req, rep);
}

void dealWithPost(Request req, Response& rep) {
	rep.setMethod(POST);
	fillPart(req, rep);
}

void dealWithDelete(Request req, Response& rep) {
	rep.setMethod(DELETE);
	checkExec(rep.getRoot() + req.getQuery());
	if (std::remove((rep.getRoot() + req.getQuery()).c_str()) != 0) {
		rep = createErrorPage(404, rep.getServ());
	} else {
		rep.setStatus("200 OK");
		rep.setContentType("text/html");
		rep.setContent("<html><body>File deleted</body></html>");
		rep.setContentLength(intToString(rep.getContent().length()));
	}
}
