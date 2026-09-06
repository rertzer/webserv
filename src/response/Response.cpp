#include "Cgi.hpp"
#include "DirListing.hpp"
#include "HttpStatus.hpp"
#include "Response.hpp"
#include "autoindex.hpp"
#include "color.hpp"
#include "files.hpp"
#include "macroDef.hpp"
#include "utils.hpp"

/* ========================== Constructor & Co ============================= */

Response::Response(Request& req)
	: _serv(*req.getServer()),
	  readFileAccess(HttpStatus::OK),
	  _autoIndex(_serv.getAutoIndex()),
	  _root(_serv.getRoot()),
	  allowed_methods(_serv.getAllowMethods()) {
	std::cerr << "RESP const autoi " << _autoIndex << std::endl;
	if (req.getCgiStatus() == CgiStatus::DONE && respWithCgi(req) == 0) {
		return;
	}
	setLocation(req.getQuery());
	if (loc.has_value() && respWithLoc(req) == 0) {
		return;
	}
	if (respWithoutLoc(req) == 0) {
		return;
	}
	dealWithMethod(req);
}

Response::Response(Server& serv, HttpStatus errcode) : _serv(serv) {
	setErrorPage(errcode);
}

Response::Response(Response const& resp) : _serv(resp._serv) {
	*this = resp;
}

Response& Response::operator=(Response const& rhs) {
	status = rhs.status;
	_contentType = rhs._contentType;
	_contentLength = rhs._contentLength;
	_connectionClose = rhs._connectionClose;
	_content = rhs._content;
	_root = rhs._root;
	root_path = rhs.root_path;
	loc = rhs.loc;
	return *this;
}

/* ================================== Getters ============================== */

std::string Response::getResponse() {
	std::string header = getResponseHeader();
	logResponse(header);

	return header + getResponseContent();
}

std::string Response::getResponseHeader() {
	std::string header;

	header += getResponseStatus();
	if (!relocation.empty()) {
		header += getResponseLocation();
	} else {
		header += getResponseConnection();
		header += getResponseContentHeader();
		header += getResponseCookies();
	}
	header += "\r\n";
	return header;
}

std::string Response::getResponseStatus() const {
	return "HTTP/1.1 " + getStatusLine(status) + "\r\n";
}

std::string Response::getResponseLocation() const {
	return "Location: " + relocation + "\r\n";
}

std::string Response::getResponseConnection() const {
	std::string connection = "";
	if (!_connectionClose.empty()) {
		connection = "Connection: " + _connectionClose + "\r\n";
	} else {
		connection = "Connection: close\r\n";
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
	return _content;
}

/* ========================================================================= */
void Response::logResponse(std::string resp) const {
	if (isStatusSuccess(status)) {
		std::cout << GREEN "\nReponse send:\n" << resp << RESET << std::endl;
	} else {
		std::cout << RED "\nReponse send:\n" << resp << RESET << std::endl;
	}
}

void Response::dealWithMethod(Request& req) {
	method = req.getMethod();
	if (isAllowed(method)) {
		if (method == GET || method == POST) {
			fillPart(req);
		} else if (method == DELETE) {
			dealWithDelete(req);
		}
	} else {
		setErrorPage(HttpStatus::METHOD_NOT_ALLOWED);
	}
}

void Response::dealWithDelete(Request& req) {
	checkRegularReadable(getRoot() + req.getQuery());
	if (std::remove((getRoot() + req.getQuery()).c_str()) != 0) {
		setErrorPage(HttpStatus::NOT_FOUND);
	} else {
		setStatus(HttpStatus::OK);
		setContentType("text/html");
		setContentWithLength("<html><body>File deleted</body></html>");
	}
}

void Response::fillPart(Request req) {
	std::string filename;
	if (req.getQuery() == "/") {
		auto default_page = getServ().getDefaultPage();
		for (auto& page : default_page) {
			filename = page;
			break;
		}
	} else {
		filename = req.getQuery();
	}

	auto fileStr = readFile(getFilePath(filename));
	if (readFileAccess == HttpStatus::OK) {
		setContentType(getContentMap().getContentValue(fileExtension(filename)));
	}

	switch (readFileAccess) {
		case HttpStatus::OK:
			fillOK(fileStr);
			break;

		default:
			setErrorPage(readFileAccess);
			break;
	}
}
std::string Response::readFile(std::string file) {
	std::string file_content;
	if (testFileAccess(file)) {
		try {
			file_content = getFileContent(file);
			setReadFileAccess(HttpStatus::OK);
		} catch (std::exception& e) {
			setReadFileAccess(HttpStatus::NOT_FOUND);
		}
	}

	return file_content;
}

bool Response::testFileAccess(std::string file) {
	bool allowed = false;
	if (!fileExists(file)) {
		setReadFileAccess(HttpStatus::NOT_FOUND);
	} else if (!isRegularFile(file) || !isReadable(file)) {
		setReadFileAccess(HttpStatus::FORBIDDEN);
	} else {
		allowed = true;
	}
	return allowed;
}

void Response::fillOK(std::string content) {
	setStatus(HttpStatus::OK);
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
	_autoIndex = autoIndexFromString(autoIndex);
}

void Response::setAllowedMethods(BitSet methods) {
	allowed_methods = methods;
}

void Response::setStatus(HttpStatus status) {
	this->status = status;
}

void Response::setMethod(HttpMethod method) {
	this->method = method;
}

void Response::setContentType(std::string contentType) {
	_contentType = contentType;
}

void Response::setContentLength(std::string contentLength) {
	_contentLength = contentLength;
}

// for CGI headers
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
	status = HttpStatus::OK;
	_connectionClose = "keep-alive";
	_contentType = "text/html";
	_content = req.getCgi()->getContent();
	extractFields();
	_contentLength = std::to_string(_content.length());
	return 0;
}

// for AutoIndex
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
	if (filedesc.isDirectory()) {
		replaceAll(content, "SIZE", "");
	} else {
		replaceAll(content, "SIZE", std::to_string(filedesc.getSize()));
	}
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
	_contentLength = std::to_string(_content.length());
}

void Response::setExtensionAllowed(std::pair<std::string, std::string> extensionAllowed) {
	_extensionAllowed = extensionAllowed;
}

void Response::setReadFileAccess(HttpStatus readFileAccess) {
	this->readFileAccess = readFileAccess;
}

void Response::setContentMap(ContentMap contentMap) {
	_contentMap = contentMap;
}

Server& Response::getServ(void) {
	return _serv;
}

std::string Response::getContentType(void) const {
	return _contentType;
}

std::string Response::getContentLength(void) const {
	return _contentLength;
}

HttpStatus Response::getStatus(void) const {
	return status;
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

std::string Response::getRoot(void) const {
	return _root;
}

std::pair<std::string, std::string> Response::getExtensionAllowed(void) const {
	return _extensionAllowed;
}

HttpStatus Response::getReadFileAccess(void) const {
	return readFileAccess;
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
	if (setRequestQuery(req)) {
		return 0;
	}

	Location& location = loc.value();
	BitSet	  allow_method = location.getAllowedMethods();
	setAllowedMethods(allow_method);
	setWithLocRoot(location);
	if (setWithLocRedirection(location, req)) {
		return 0;
	}
	if (!location.getExtension().empty() && req.getExtension() == location.getExtension()) {
		req.initCgi(getRoot(), location);
		return 0;
	} else {
		req.setUploadPath(location.getUploadPath());
		if (req.isUpload()) {
			req.uploadAll();
		}
	}
	return 1;
}

bool Response::setRequestQuery(Request& req) {
	Location& location = loc.value();
	auto	  req_query = req.getQuery();

	if (req_query != "/") {
		if (req_query.back() == '/') {
			if (_autoIndex == AutoIndex::ON) {
				createAutoIndexResp(req, location);
				return true;
			} else {
				if (getSpecIndex() == "")
					req_query = "/";
				else
					req_query = getSpecIndex();
			}
		}
		req.setQuery(req_query);
	}
	return false;
}

void Response::setWithLocRoot(Location& loc) {
	if (loc.isRooted()) {
		setRoot(loc.getRoot());
		root_path = loc.getLocationPath();
	}
}

bool Response::setWithLocRedirection(Location& loc, Request& req) {
	bool redir = false;
	if (loc.checkForRedirection()) {
		setStatus(loc.getRedirectionStatus());
		relocation = loc.getRedirectionPath();
		std::cout << GREEN "Redir = {[Status :" << static_cast<int>(getStatus())
				  << "][New Location: " << relocation << "]" RESET << std::endl;
		req.setKeepAlive(false);
		redir = true;
	}
	return redir;
}

int Response::respWithoutLoc(Request& req) {
	if (req.getQuery() != "/" && req.getQuery().back() == '/') {
		if (_autoIndex == AutoIndex::ON) {
			createAutoIndexResp(req, Location());
			return 0;
		} else
			req.setQuery("/");
	}
	return 1;
}

void Response::createAutoIndexResp(Request& req, Location loc) {
	BitSet allow_method = loc.getAllowedMethods();
	if (allow_method.getFlags() != 0) {
		setAllowedMethods(allow_method);
	}

	auto method = req.getMethod();
	if ((method == GET || method == POST) && (isAllowed(method))) {
		setContentWithLength(getDirContent(req.getQuery()));
		setStatus(HttpStatus::OK);
		setMethod(method);
		setContentType("text/html");
	} else {
		setErrorPage(HttpStatus::METHOD_NOT_ALLOWED);
	}
}

std::string Response::getSpecIndex() {
	auto item = loc->get().getIndex();
	auto path = _root + item;
	if (item.empty() || !isRegularFile(path) || !isReadable(path)) {
		item.erase();
	}
	return item;
}

void Response::setLocation(std::string path) {
	path = extractDirPath(path);
	loc = _serv.findLocation(path);
	std::cerr << "setLocation\n";
	if (loc.has_value()) {
		AutoIndex ai = loc->get().getAutoindex();
		std::cerr << "loc value " << ai << "\n";
		if (ai != AutoIndex::NONE) {
			_autoIndex = ai;
		}
		std::cerr << "loc value " << _autoIndex << "\n";
	}
}

std::string Response::getFilePath(std::string const& file) const {
	if (!root_path.empty() && file.starts_with(root_path)) {
		return _root + file.substr(root_path.size());
	}
	return _root + file;
}

void Response::setErrorPage(HttpStatus error) {
	std::string content;
	status = error;
	_contentType =
		ContentMap().getContentValue(fileExtension(_serv.getErrorPage(static_cast<int>(status))));
	try {
		content = readSpecFile(_serv.getRoot() + _serv.getErrorPage(static_cast<int>(status)));
	} catch (const std::exception& e) {
		content = "<html><body>File deleted</body></html>";
	}
	setContentWithLength(content);
	_connectionClose = "close";
	_setCookie.clear();
}
