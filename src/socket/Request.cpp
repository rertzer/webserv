#include <fstream>
#include <sstream>

#include "Cgi.hpp"
#include "color.hpp"
#include "ErrorException.hpp"
#include "HttpMethod.hpp"
#include "Request.hpp"
#include "Connection.hpp"
#include "files.hpp"

Request::Request(Connection* s)
	: port(s->getListeningSocketPort()),
	  body_size(1000000),
	  soc(s),
	  cgi(nullptr),
	  header_ok(false),
	  content_ok(false),
	  server(nullptr)

{

	std::cout << " ------------------------------ " << std::endl << "\n";
	int len = soc->readAll();
	if (len == 0)
		throw(RequestException());
	setControlData();
	setServer();
	setHeader();
	if (contentExist()) {
		setContent();
	}
}

Request::Request(Request const& rhs) {
	*this = rhs;
}

Request::~Request() {
	if (cgi != nullptr) {
		delete cgi;
		cgi = nullptr;
	}
}

Request& Request::operator=(Request const& rhs) {
	if (this != &rhs) {
		port = rhs.port;
		body_size = rhs.body_size;
		soc = rhs.soc;
		cgi = rhs.cgi;
		header = rhs.header;
		trailer = rhs.trailer;
		multipart = rhs.multipart;
		query = rhs.query;
		method = rhs.method;
		content = rhs.content;
		header_ok = rhs.header_ok;
		content_ok = rhs.content_ok;
		server = rhs.server;
	}
	return *this;
}

int Request::getPort() const {
	return port;
}

CgiStatus Request::getCgiStatus() const {
	if (cgi != nullptr)
		return cgi->getStatus();
	else
		return CgiStatus::NO_INIT;
}

const std::string& Request::getProtocol() const {
	return protocol;
}

HttpMethod Request::getMethod() const {
	return method;
}

const std::string& Request::getQuery() const {
	return query;
}

std::string Request::getField(std::string const& name) const {
	auto it = header.find(name);
	if (it == header.end()) {
		it = trailer.find(name);
		if (it == trailer.end())
			return "";
	}
	return it->second;
}

unsigned int Request::getBodySize() const {
	return body_size;
}

Cgi* Request::getCgi() const {
	return cgi;
}

Server* Request::getServer(){
	return server;
}

void Request::setBodySize(int bs) {
	if (bs > 0)
		body_size = bs;
}

void Request::setUploadPath(std::string up) {
	upload_path = up;
}

bool Request::checkField(std::string const& name, std::string const& value) const {
	auto field = getField(name);
	auto all_values = splitCsv(field, ";");
	for (auto csv_value : all_values) {
		if (ciCompare(csv_value, value))
			return true;
	}
	return false;
}

bool Request::isUpload() const {
	if (getMethod() == POST && checkField("Content-Type", "multipart/form-data") &&
		!upload_path.empty()) {
		return true;
	}
	return false;
}

void Request::upload_all() {
	auto boundary = getLine("\r\n");
	auto part = getLine(boundary);
	multipart.clear();
	upload(part);
}

void Request::upload(std::string& part) {
	std::string line = getLine(part, "\r\n");
	while (line.length()) {
		addMultipart(line);
		line = getLine(part, "\r\n");
	}
	std::string filename = getFileName();
	if (!filename.empty())
		uploadFile(filename, part);
}

void Request::addMultipart(std::string& line) {
	auto kv = splitPair(line, ':');
	if (kv.first.empty()) {
		throw(ErrorException(400));
	}
	stringTrim(kv.first);
	stringTrim(kv.second);
	multipart[kv.first] = kv.second;
}

std::string Request::getFileName() {
	std::string fn = multipart["Content-Disposition"];
	for (auto& field : splitCsv(fn, ";")) {
		auto kv = splitPair(field, '=');
		stringDoubleQuotTrim(kv.first);
		stringDoubleQuotTrim(kv.second);
		if (kv.first == "filename") {
			fn = kv.second;
			break;
		}
	}
	return fn;
}

std::optional<std::string> Request::getExtension() const {
	std::optional<std::string> extension;
	auto					   begin = query.find(".");

	if (begin != std::string::npos) {
		auto end = getExtensionEnd(begin);
		extension = query.substr(begin, end - begin);
	}
	return extension;
}

size_t Request::getExtensionEnd(size_t begin) const {
	auto end = query.find('?', begin);
	if (end == std::string::npos) {
		end = query.find('/', begin);
	}
	return end;
}

void Request::initCgi(std::string root, Location& loc) {
	cgi = new Cgi(root, *this, loc.getExtension(), loc.getCgiPath());
}

void Request::uploadFile(std::string const& filename, std::string const& part) {
	checkValidFileName(filename);
	std::string path = upload_path + filename;
	if (fileExists(path))
		std::cout << "File " << path << " already exist\n";
	else {
		std::ofstream upfile(path.c_str(), std::ofstream::out);
		if (upfile.fail()) {
			perror(" creation failed");
			throw(ErrorException(500));
		}
		upfile << part;
		upfile.close();
	}
}

void Request::checkValidFileName(std::string const& filename) const {
	if (filename.size() > 255 || filename.find_first_of("\\\0") != std::string::npos ||
		filename == "." || filename == ".."){

		throw ErrorException(400);
	}
}

std::string Request::getLine(std::string const& sep) {
	std::string line;

	auto pos = content.find(sep);
	if (pos != std::string::npos) {
		line = content.substr(0, pos);
		content.erase(0, pos + sep.length());
	}
	return line;
}

bool Request::ready() const {
	return header_ok && ((contentExist() && content_ok) || !contentExist());
}

void Request::feed() {
	if (soc->readAll() == 0) {
		throw(RequestException());
	}
	if (!header_ok) {
		setHeader();
	}
	if (header_ok && contentExist() && !content_ok) {
		setContent();
	}
}

void Request::eraseContent(int size) {
	content.erase(0, size);
}

Connection* Request::getSocket() const {
	return soc;
}

std::string Request::getLine(std::string& data, std::string const& sep) {
	std::string line;

	auto pos = data.find(sep);
	if (pos != std::string::npos) {
		line = data.substr(0, pos);
		data.erase(0, pos + sep.length());
	}
	return line;
}

size_t Request::getContentLength() const {
	auto len = toInt(getField("Content-Length"));
	if (!len.has_value()) {
		len = 0;
	}
	return len.value();
}

const std::map<std::string, std::string>& Request::getHeader() const {
	return header;
}

const std::map<std::string, std::string>& Request::getTrailer() const {
	return trailer;
}

const std::string& Request::getContent() const {
	return content;
}

void Request::addField(std::string const& field) {
	auto kv = splitPair(field, ':');
	if (kv.first.empty()) {
		throw(ErrorException(400));
	}
	stringTrim(kv.first);
	stringTrim(kv.second);
	if (header.find(kv.first) == header.end())
		header[kv.first] = kv.second;
	else
		header[kv.first] += ", " + kv.second;
}

// Private
void Request::setControlData() {
	std::string line = soc->getLine();

	if (line.empty()) {
		line = soc->getLine();
	}

	auto control_data = split(line);
	if (control_data.size() != 3) {
		throw(ErrorException(400));
	}

	method = stringToMethod(control_data[0]);
	query = control_data[1];
	protocol = control_data[2];

	checkControlData();
}

void Request::setQuery(std::string const& q) {
	query = q;
}

void Request::setHeader() {
	setFields();
	setBodySize(server->getBodySize());
	checkHeader();
	setKeepAlive();
	header_ok = true;
}

void Request::setKeepAlive() {
	std::string keep = getField("Connection");
	if (keep == "keep-alive")
		soc->setKeepAlive(true);
}

void Request::setCgi(Cgi* c) {
	cgi = c;
}

void Request::setServer(){
for (auto& serv : soc->getServers()) {
		if (getField("Host") == serv.getServName() + ":" + std::to_string(getPort())) {
				server = &serv;
				return;
		}
	};
	server = soc->getDefaultServer();
}

void Request::setFields() {
	std::string line = soc->getLine();

	while (line.length()) {
		addField(line);
		line = soc->getLine();
	}
}

void Request::setContent() {
	std::string trans_encoding = getField("Transfer-Encoding");

	if (!trans_encoding.empty()) {
		if (checkField("Transfer-Encoding", "chunked"))
			setContentByChunked();
		else
			throw(ErrorException(501));
	} else {
		setContentByLength();
	}
}

void Request::setContentByChunked() {
	int len = 1;
	while (len) {
		len = readChunk();
	}
	std::stringstream ss;
	ss << content.size();
	header.erase(header.find("Transfer-Encoding"));
	header["Content-Length"] = ss.str().c_str();
	setTrailer();
}

unsigned int Request::readChunk() {
	std::stringstream ss;
	ss << std::hex << soc->getLine();
	unsigned int size = 0;
	if (ss >> size) {
		if (content.size() + size > body_size)
			throw(ErrorException(413));
		soc->addRawData(content, size);
		soc->getLine();
		if (size == 0)
			content_ok = true;
	}
	return size;
}

void Request::setTrailer() {
	setFields();
}

void Request::setContentByLength() {
	size_t	len = getContentLength();
	ssize_t remain = len - content.size();
	if (remain > 0) {
		soc->addRawData(content, remain);
	}
	if (content.size() == len)
		content_ok = true;
}

void Request::checkControlData() const {
	if (protocol != "HTTP/1.1") {
		throw(ErrorException(505));
	}
	if (method == NONE) {
		throw(ErrorException(400));
	}
	if (method == PUT || method == HEAD)
		throw(ErrorException(501));
}

void Request::checkHeader() const {
	if (getField("Host").empty()){
		throw(ErrorException(400));
	}
	if (getContentLength() > body_size) {
		throw(ErrorException(413));
	}
}

bool Request::contentExist() const {
	int content = 0;
	if (!getField("Transfer-Encoding").empty())
		content++;
	if (!getField("Content-Length").empty())
		content++;
	if (content == 2) {

		throw(ErrorException(400));
	}
	return static_cast<bool>(content);
}


void Request::printCleanRequest() const {
	std::cout << "\n" << CYAN "Request = {";
	std::cout << "[Method: " << getMethod() << "]";
	std::cout << "[File: " << getQuery() << "]";
	std::cout << "[Port: " << getPort() << "]";
	std::cout << "[Host: " << getField("Host") << "]";
	std::cout << "}" RESET << std::endl;
}

// Non member functions

std::ostream& operator<<(std::ostream& ost, Request const& rhs) {
	ost << "port: " << rhs.getPort() << "\n";
	ost << "protocol: " << rhs.getProtocol() << "\n";
	ost << "method: " << rhs.getMethod() << "\n";
	ost << "query: " << rhs.getQuery() << "\n";
	ost << "Body size: " << rhs.getBodySize() << "\n";
	ost << "Header:\n";
	for (auto& header : rhs.getHeader())
		ost << "\t" << header.first << ": " << header.second << "\n";
	ost << "Trailer:\n";
	for (auto& trailer : rhs.getTrailer())
		ost << "\t" << trailer.first << ": " << trailer.second << "\n";
	ost << "content: " << rhs.getContent() << "\n";
	return ost;
}

