#include "Response.hpp"
#include "Status.hpp"
#include "TCPSocket.hpp"
#include "color.hpp"
#include "macroDef.hpp"

int respWithLoc(Request& req, Response& rep) {
	Location loc = getTheLocation(req.getQuery(), rep);
	if (req.getQuery() != "/") {
		if (req.getQuery()[req.getQuery().length() - 1] == '/') {
			if (checkAutoIndex(loc) == 0) {
				if (getSpecIndex(loc, rep) == "")
					req.setQuery("/");
				else
					req.setQuery(getSpecIndex(loc, rep));
				loc = getTheLocation(req.getQuery(), rep);
			} else if (checkAutoIndex(loc) == 1 || rep.getAutoIndex() == "on") {
				createAutoIndexResp(req, loc, rep);
				return 0;
			} else
				req.setQuery("/");
		}
	}

	BitSet allow_method = checkAllowMethod(loc);
	rep.setAllowedMethods(allow_method);
	if (isThereAspecRoot(loc) == 1) {
		rep.setRoot(
			getArgsLoc(loc, "root")
				.substr(0, getArgsLoc(loc, "root").length() - loc.getLocationPath().length()));
	}
	if (checkForRedirection(loc) == 1) {
		std::pair<std::string, std::string> redirection = RedirectTo(loc);
		rep.setStatus(Status::getMsg(atoi((redirection.first.c_str()))));
		rep.setLocation(redirection.second.substr(0, redirection.second.length()));
		std::cout << GREEN "Redir = {[Status :" << rep.getStatus()
				  << "][New Location: " << rep.getLocation() << "]" RESET << std::endl;
		req.getSocket()->setKeepAlive(false);
		return 0;
	}
	if (!getExtension(loc).first.empty() && req.getExtension() == getExtension(loc).first)
		return initCgi(req, loc, rep);
	else {
		req.setUploadPath(getUploadPath(loc));
		if (req.isUpload()) {
			req.upload_all();
		}
	}
	return 1;
}

int respWithOutLoc(Request& req, Response& rep) {
	if (req.getQuery() != "/") {
		if (req.getQuery()[req.getQuery().length() - 1] == '/') {
			if (rep.getAutoIndex() == "on") {
				createAutoIndexResp(req, Location(), rep);
				return 0;
			} else
				req.setQuery("/");
		}
	}
	return 1;
}
