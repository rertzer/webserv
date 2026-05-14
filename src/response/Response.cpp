#include "Response.hpp"
#include "Cgi.hpp"
#include "DirListing.hpp"
#include "Status.hpp"
#include "TCPSocket.hpp"
#include "autoindex.hpp"
#include "color.hpp"
#include "files.hpp"
#include "macroDef.hpp"
#include "utils.hpp"

Response::Response(Request& req)
	: _serv(*req.getServer()),
	  _readFileAccess(OK),
	  _autoIndex(_serv.getAutoIndex()),
	  _root(_serv.getRoot()),
	  allowed_methods(_serv.getAllowMethods()) {
	if (req.getCgiStatus() == CgiStatus::DONE && respWithCgi(req) == 0) {
		return;
	}
	if (checkIfLocation(req.getQuery()) && respWithLoc(req) == 0) {
		return;
	}
	if (respWithoutLoc(req) == 0) {
		return;
	}
	dealWithMethod(req);
}
Response::Response(Server& serv, int errcode) : _serv(serv) {
	setErrorPage(errcode);
}

Response::Response(Response const& resp) : _serv(resp._serv) {
	*this = resp;
}

Response& Response::operator=(Response const& rhs) {
	_status = rhs._status;
	_contentType = rhs._contentType;
	_contentLength = rhs._contentLength;
	_connectionClose = rhs._connectionClose;
	_content = rhs._content;
	_root = rhs._root;
	root_path = rhs.root_path;
	return *this;
}

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

void Response::dealWithMethod(Request& req) {
	method = req.getMethod();
	if (isAllowed(method)) {
		if (method == GET || method == POST) {
			fillPart(req);
		} else if (method == DELETE) {
			dealWithDelete(req);
		}
	} else {
		setErrorPage(405);
	}
}

void Response::dealWithDelete(Request& req) {
	checkRegularReadable(getRoot() + req.getQuery());
	if (std::remove((getRoot() + req.getQuery()).c_str()) != 0) {
		setErrorPage(404);
	} else {
		setStatus("200 OK");
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
	if (_readFileAccess == OK) {
		setContentType(getContentMap().getContentValue(fileExtension(filename)));
	}

	switch (_readFileAccess) {
		case OK:
			fillOK(fileStr);
			break;
		case ACCESS_DENIED:
			setErrorPage(403);
			break;
		case FILE_NOT_FOUND:
			setErrorPage(404);
			break;
		default:
			setErrorPage(500);
			break;
	}
}
std::string Response::readFile(std::string file) {
	std::string file_content;
	if (testFileAccess(file)) {
		try {
			file_content = getFileContent(file);
			setReadFileAccess(OK);
		} catch (std::exception& e) {
			setReadFileAccess(FILE_NOT_FOUND);
		}
	}

	return file_content;
}

bool Response::testFileAccess(std::string file) {
	bool allowed = false;
	if (!fileExists(file)) {
		setReadFileAccess(FILE_NOT_FOUND);
	} else if (!isRegularFile(file) || !isReadable(file)) {
		setReadFileAccess(ACCESS_DENIED);
	} else {
		allowed = true;
	}
	return allowed;
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
	_autoIndex = autoIndexFromString(autoIndex);
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
	_status = "200 OK";
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
	if (filedesc.isDirectory()){
		replaceAll(content, "SIZE", "");
	}
	else {
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

void Response::setReadFileAccess(int readFileAccess) {
	_readFileAccess = readFileAccess;
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

AutoIndex Response::getAutoIndex(void) const {
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
	Location loc = getTheLocation(req.getQuery());

	if (setRequestQuery(loc, req)) {
		return 0;
	}

	BitSet allow_method = loc.getAllowedMethods();
	setAllowedMethods(allow_method);
	setWithLocRoot(loc);
	if (setWithLocRedirection(loc, req)) {
		return 0;
	}
	if (!loc.getExtension().empty() && req.getExtension() == loc.getExtension()) {
		req.initCgi(getRoot(), loc);
		return 0;
	} else {
		req.setUploadPath(loc.getUploadPath());
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
			if (loc.getAutoindex() == AutoIndex::OFF) {
				if (getSpecIndex(loc) == "")
					req_query = "/";
				else
					req_query = getSpecIndex(loc);
				loc = getTheLocation(req_query);
			} else if (loc.getAutoindex() == AutoIndex::ON || _autoIndex == AutoIndex::ON) {
				createAutoIndexResp(req, loc);
				return true;
			} else
				req_query = "/";
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
		setStatus(Status::getMsg(loc.getRedirectionStatus()));
		setLocation(loc.getRedirectionPath());
		std::cout << GREEN "Redir = {[Status :" << getStatus()
				  << "][New Location: " << getLocation() << "]" RESET << std::endl;
		req.getSocket()->setKeepAlive(false);
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
		setStatus("200 OK");
		setMethod(method);
		setContentType("text/html");
	} else {
		std::cerr << "fill response 88\n";
		setErrorPage(405);
	}
}

std::string Response::getSpecIndex(Location loc) {
	auto item = loc.getIndex();
	auto path = _root + item;
	if (item.empty() || !isRegularFile(path) || !isReadable(path)) {
		item.erase();
	}
	return item;
}

Location Response::getTheLocation(std::string path) {
	path = extractDirPath(path);
	auto loc = getServ().findLocation(path);

	if (loc.has_value()) {
		return loc.value();
	}
	return Location();
}

int Response::checkIfLocation(std::string path) {
	if (path != "/") {
		path = path.substr(0, path.rfind("."));
	}

	auto loc = _serv.findLocation(path);
	return loc.has_value();
}

std::string Response::getFilePath(std::string const& file) const {
	if (!root_path.empty() && file.starts_with(root_path)) {
		return _root + file.substr(root_path.size());
	}
	return _root + file;
}

void Response::setErrorPage(int errcode) {
	std::string content;
	_status = Status::getMsg(errcode);
	_contentType = ContentMap().getContentValue(fileExtension(_serv.getErrorPage(errcode)));
	try {
		content = readSpecFile(_serv.getRoot() + _serv.getErrorPage(errcode));
	} catch (const std::exception& e) {
		content = "<html><body>File deleted</body></html>";
	}
	setContentWithLength(content);
	_connectionClose = "close";
	_setCookie.clear();
}
