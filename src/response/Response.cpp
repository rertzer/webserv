#include "Response.hpp"
#include "Cgi.hpp"
#include "DirListing.hpp"
#include "Status.hpp"
#include "TCPSocket.hpp"
#include "color.hpp"
#include "macroDef.hpp"

std::string Response::getResponse() {
	std::string header = getResponseHeader();
	logResponse(header);

	return header + getResponseContent();
}

std::string Response::getResponseHeader() {
	std::string header;

	header += getResponseStatus();
	if (!_location.empty()) {
		header += getResponseLocation();
	} else {
		header += getResponseConnection();
		header += getResponseContentHeader();
		header += getResponseCookies();
	}
	return header;
}

std::string Response::getResponseStatus() const {
	return "HTTP/1.1 " + _status + " \r\n";
}

std::string Response::getResponseLocation() const {
	return "Location: " + _location + "\r\n";
}

std::string Response::getResponseConnection() const {
	std::string connection = "";
	if (!_connectionClose.empty()) {
		connection = "Connection: " + _connectionClose + "\r\n";
	}
	return connection;
}

std::string Response::getResponseContentHeader() const {
	return "Content-Type: " + _contentType + "\r\n" + "Content-Length: " + _contentLength + "\r\n";
}

std::string Response::getResponseCookies() const {
	int			cookie_nb = _setCookie.size();
	std::string cookies = "";
	for (int i = 0; i < cookie_nb; i++) {
		cookies += "Set-Cookie: " + _setCookie[i] + "\r\n";
	}
	return cookies;
}

std::string Response::getResponseContent() const {
	return "\r\n" + _content;
}

void Response::logResponse(std::string resp) const {
	if (_status == "200 OK")
		std::cout << GREEN "\nReponse send:\n" << resp << RESET << std::endl;
	else
		std::cout << RED "\nReponse send:\n" << resp << RESET << std::endl;
}

Response::Response(Request& req, Server& serv) {
	_readFileAccess = OK;
	_serv = serv;
	_root = _serv.getRoot();
	_autoIndex = _serv.getAutoIndex();
	allowed_methods = serv.getAllowMethods();
	if (req.getCgiStatus() == CgiStatus::DONE && respWithCgi(req) == 0) {
		return;
	}
	if (checkIfLocation(req.getQuery(), *this) != -1 && respWithLoc(req) == 0) {
		return;
	}
	if (respWithoutLoc(req) == 0) {
		return;
	}
	dealWithMethod(req);
}

void Response::dealWithMethod(Request& req) {
	method = req.getMethod();
	if (isAllowed(method)) {
		if (method == GET || method == POST) {
			fillPart(req, *this);
		} else if (method == DELETE) {
			dealWithDelete(req);
		}
	} else {
		*this = createErrorPage(405, _serv);
	}
}

void Response::dealWithDelete(Request& req) {
	checkExec(getRoot() + req.getQuery());
	if (std::remove((getRoot() + req.getQuery()).c_str()) != 0) {
		*this = createErrorPage(404, getServ());
	} else {
		setStatus("200 OK");
		setContentType("text/html");
		setContentWithLength("<html><body>File deleted</body></html>");
	}
}
Response::Response(std::string status,
				   std::string contentType,
				   std::string contentLength,
				   std::string connectionClose,
				   std::string content) {
	_status = status;
	_contentType = contentType;
	_contentLength = contentLength;
	_connectionClose = connectionClose;
	_content = content;
}

Response::Response(Response const& resp) {
	*this = resp;
}

Response& Response::operator=(Response const& rhs) {
	_status = rhs._status;
	_contentType = rhs._contentType;
	_contentLength = rhs._contentLength;
	_connectionClose = rhs._connectionClose;
	_content = rhs._content;
	return *this;
}

void Response::fillOK(std::string content) {
	setStatus("200 OK");
	setContentWithLength(content);
	setConnectionClose("keep-alive");
}

void Response::Setserv(Server serv) {
	_serv = serv;
}

void Response::setRoot(std::string root) {
	_root = root;
}

void Response::setAutoIndex(std::string autoIndex) {
	_autoIndex = autoIndex;
}

void Response::setAllowedMethods(BitSet methods) {
	allowed_methods = methods;
}

void Response::setLocation(std::string location) {
	_location = location;
}

void Response::setStatus(std::string status) {
	_status = status;
}

void Response::setMethod(HttpMethod m) {
	method = m;
}

void Response::setContentType(std::string contentType) {
	_contentType = contentType;
}

void Response::setContentTypeByRequest(Request const& req) {
	_contentType = getContentMap().getContentValue(
		req.getQuery().substr(req.getQuery().rfind(".") + 1, req.getQuery().length()));
}

void Response::setContentLength(std::string contentLength) {
	_contentLength = contentLength;
}

std::pair<std::string, std::string> Response::extractField(size_t pos) {
	std::string							line;
	std::pair<std::string, std::string> field;

	if (pos != std::string::npos) {
		line = _content.substr(0, pos);
		_content.erase(0, pos + 2);
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

int Response::respWithCgi(Request& req) {
	method = req.getMethod();
	_status = "200 OK";
	_connectionClose = "keep-alive";
	_contentType = "text/html";
	_content = req.getCgi()->getContent();
	extractFields();
	_contentLength = intToString(_content.length());
	return 0;
}

void Response::extractFields() {
	size_t pos = _content.find("\r\n");
	while (pos != std::string::npos && pos != 0) {
		auto field = extractField(pos);
		if (field.first == "Content-Type") {
			_contentType = field.second;
		} else if (field.first == "Set-Cookie") {
			_setCookie.push_back(field.second);
		}
		pos = _content.find("\r\n");
	}
}

std::string Response::getDirContent(std::string path) {
	DirListing drl(_root + path);
	auto	   index = _serv.getHtmlCode(HtmlCode::AUTOINDEX_HEADER);

	replaceAll(index, "PATH", path);

	auto content = _serv.getHtmlCode(HtmlCode::AUTOINDEX_CONTENT);
	for (auto& filedesc : drl.getDirContent()) {
		index += appendDirContent(content, filedesc);
	}
	auto footer = _serv.getHtmlCode(HtmlCode::AUTOINDEX_FOOTER);
	index.append(footer);
	return index;
}

std::string Response::appendDirContent(std::string content, FileDesc const& filedesc) {
	replaceAll(content, "NAME", filedesc.getName());
	replaceAll(content, "TYPE", filedesc.getTypeName());
	replaceAll(content, "SIZE", std::to_string(filedesc.getSize()));
	replaceAll(content, "LASTMODIFIED", filedesc.getLastModified());
	return content;
}

void Response::setConnectionClose(std::string connectionClose) {
	_connectionClose = connectionClose;
}

void Response::setContent(std::string content) {
	_content = content;
}

void Response::setContentWithLength(std::string content) {
	_content = content;
	_contentLength = intToString(_content.length());
}

void Response::setExtensionAllowed(std::pair<std::string, std::string> extensionAllowed) {
	_extensionAllowed = extensionAllowed;
}

void Response::setReadFileAccess(int readFileAccess) {
	_readFileAccess = readFileAccess;
}

void Response::setContentMap(ContentMap contentMap) {
	_contentMap = contentMap;
}

Server Response::getServ(void) const {
	return _serv;
}

std::string Response::getContentType(void) const {
	return _contentType;
}

std::string Response::getContentLength(void) const {
	return _contentLength;
}

std::string Response::getStatus(void) const {
	return _status;
}

HttpMethod Response::getMethod(void) const {
	return method;
}

std::string Response::getConnectionClose(void) const {
	return _connectionClose;
}

std::string Response::getContent(void) const {
	return _content;
}

std::string Response::getLocation(void) const {
	return _location;
}

std::string Response::getRoot(void) const {
	return _root;
}

std::string Response::getAutoIndex(void) const {
	return _autoIndex;
}

std::pair<std::string, std::string> Response::getExtensionAllowed(void) const {
	return _extensionAllowed;
}

int Response::getReadFileAccess(void) const {
	return _readFileAccess;
}

BitSet Response::getAllowedMethods(void) const {
	return allowed_methods;
}

std::vector<std::string> Response::getCookie(void) const {
	return _setCookie;
}

ContentMap Response::getContentMap(void) const {
	return _contentMap;
}

bool Response::isAllowed(HttpMethod m) const {
	return allowed_methods.isSet(m);
}

int Response::respWithLoc(Request& req) {
	Location loc = getTheLocation(req.getQuery(), *this);

	if (setRequestQuery(loc, req)) {
		return 0;
	}

	BitSet allow_method = checkAllowMethod(loc);
	setAllowedMethods(allow_method);
	setWithLocRoot(loc);
	if (setWithLocRedirection(loc, req)) {
		return 0;
	}
	if (!getExtension(loc).first.empty() && req.getExtension() == getExtension(loc).first)
		return initCgi(req, loc, *this);
	else {
		req.setUploadPath(getUploadPath(loc));
		if (req.isUpload()) {
			req.upload_all();
		}
	}
	return 1;
}

bool Response::setRequestQuery(Location& loc, Request& req) {
	auto req_query = req.getQuery();
	if (req_query != "/") {
		if (req_query.back() == '/') {
			if (checkAutoIndex(loc) == 0) {	 // autoindex off
				if (getSpecIndex(loc, *this) == "")
					req_query = "/";
				else
					req_query = getSpecIndex(loc, *this);
				loc = getTheLocation(req_query, *this);
			} else if (checkAutoIndex(loc) == 1 || getAutoIndex() == "on") {
				createAutoIndexResp(req, loc, *this);
				return true;
			} else
				req_query = "/";
		}
		req.setQuery(req_query);
	}
	return false;
}

void Response::setWithLocRoot(Location& loc) {
	if (isThereAspecRoot(loc) == 1) {
		setRoot(getArgsLoc(loc, "root")
					.substr(0, getArgsLoc(loc, "root").length() - loc.getLocationPath().length()));
	}
}

bool Response::setWithLocRedirection(Location& loc, Request& req) {
	bool redir = false;
	if (checkForRedirection(loc) == 1) {
		std::pair<std::string, std::string> redirection = RedirectTo(loc);
		setStatus(Status::getMsg(atoi((redirection.first.c_str()))));
		setLocation(redirection.second.substr(0, redirection.second.length()));
		std::cout << GREEN "Redir = {[Status :" << getStatus()
				  << "][New Location: " << getLocation() << "]" RESET << std::endl;
		req.getSocket()->setKeepAlive(false);
		redir = true;
	}
	return redir;
}

int Response::respWithoutLoc(Request& req) {
	if (req.getQuery() != "/" && req.getQuery().back() == '/') {
		if (getAutoIndex() == "on") {
			createAutoIndexResp(req, Location(), *this);
			return 0;
		} else
			req.setQuery("/");
	}
	return 1;
}
