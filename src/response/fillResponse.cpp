#include "Cgi.hpp"
#include "ErrorException.hpp"
#include "Response.hpp"
#include "macroDef.hpp"
#include "utils.hpp"

int isDir(std::string fileName) {
	struct stat path;

	memset(&path, 0, sizeof(path));
	stat(fileName.c_str(), &path);

	return S_ISREG(path.st_mode);
}

std::string readFile(std::string file, Response& rep) {
	std::string file_content;
	if (isDir(file) == 0) {
		throw(ErrorException(404));
	}
	if (access(file.c_str(), F_OK) == -1) {
		rep.setReadFileAccess(FILE_NOT_FOUND);
		return "404";
	}
	if (access(file.c_str(), R_OK) == -1) {
		rep.setReadFileAccess(ACCESS_DENIED);
		return "403";
	}
	try {
		file_content = getFileContent(file);
	} catch (std::exception& e) {
		rep.setReadFileAccess(FILE_NOT_FOUND);
		return "404";
	}
	rep.setReadFileAccess(OK);
	return file_content;
}

void fillPart(Request req, Response& rep) {
	if (req.getQuery() == "/") {
		std::string fileStr;
		auto		default_page = rep.getServ().getDefaultPage();
		for (auto& page : default_page) {
			fileStr = readFile(rep.getRoot() + page, rep);
			if (fileStr == "403" && rep.getReadFileAccess() == ACCESS_DENIED) {
				break;
			}
			if (fileStr != "404" && rep.getReadFileAccess() != FILE_NOT_FOUND) {
				rep.setContentType(rep.getContentMap().getContentValue(
					page.substr(page.rfind(".") + 1, page.length())));
				break;
			}
		}
		if (fileStr == "404" && rep.getReadFileAccess() == FILE_NOT_FOUND)
			rep = createErrorPage(404, rep.getServ());
		else if (fileStr == "403" && rep.getReadFileAccess() == ACCESS_DENIED)
			rep = createErrorPage(403, rep.getServ());
		else {
			rep.fillOK(fileStr);
		}
	} else {
		std::string fileStr;
		fileStr = readFile(rep.getRoot() + req.getQuery(), rep);
		if (fileStr == "404" && rep.getReadFileAccess() == FILE_NOT_FOUND)
			throw(ErrorException(404));
		else if (fileStr == "403" && rep.getReadFileAccess() == ACCESS_DENIED)
			rep = createErrorPage(403, rep.getServ());
		else {
			rep.fillOK(fileStr);
			rep.setContentTypeByRequest(req);
		}
	}
}

void createAutoIndexResp(Request& req, Location loc, Response& rep) {
	BitSet allow_method = checkAllowMethod(loc);
	if (allow_method.getFlags() != 0)
		rep.setAllowedMethods(allow_method);
	if ((req.getMethod() == "GET" && rep.isAllowed(GET)) ||
		(req.getMethod() == "POST" && (rep.isAllowed(POST)))) {
		rep.setContent(rep.getDirContent(req.getQuery()));
		rep.setStatus("200 OK");
		rep.setMethod(req.getMethod());
		rep.setContentType("text/html");
		rep.setContentLength(intToString(rep.getContent().length()));
	} else {
		rep = createErrorPage(405, rep.getServ());
	}
}

void setCookie(std::string ck, Response& rep) {
	rep.getCookie().push_back(ck);
}

int initCgi(Request& req, Location& loc, Response& rep) {
	Cgi* myCgi = new Cgi(req.getMethod(), rep.getRoot(), req, getExtension(loc));
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
