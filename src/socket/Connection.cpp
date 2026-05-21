#include <cstring>

#include "Connection.hpp"
#include "ErrorException.hpp"
#include "ServerException.hpp"
#include "Request.hpp"

/* ============================== Coplien Methods =========================== */
// Used for listening sockets
Connection::Connection(int port) :listening_port(port), keep_alive(true), error(false), listening(true), request(nullptr), default_server(nullptr),  soc(TcpSocket(port)) {
	}

// Used for non listening sockets
Connection::Connection()
	: listening_port(0), keep_alive(false), error(false), listening(false), request(nullptr), soc(TcpSocket()){
	
}

Connection::Connection(Connection const& rhs) {
	*this = rhs;
}

Connection::~Connection() {
	if (request) {
		delete request;
		request = nullptr;
	}
}

Connection& Connection::operator=(Connection const& rhs) {
	if (this != &rhs) {
	
		listening_port = rhs.listening_port;
		msg_in = rhs.msg_in;
		msg_out = rhs.msg_out;
		request = rhs.request;
		keep_alive = rhs.keep_alive;
		error = rhs.error;
		listening = rhs.listening;
		servers = rhs.servers;
		default_server= rhs.default_server;
		soc = rhs.soc;
	}
	return *this;
}

/* ============================= Getters ==================================== */

int Connection::getPort() const {
	return soc.getPort();
}

int Connection::getListeningSocketPort() const {
	return listening_port;
}

int Connection::getSocketFd() const {
	return soc.getFd();
}

bool Connection::getError() const {
	return error;
}

std::string Connection::getMessageIn() const {
	return msg_in;
}

std::string Connection::getMessageOut() const {
	return msg_out;
}

bool Connection::getKeepAlive() const {
	return keep_alive;
}

Server* Connection::getDefaultServer() const{
	return default_server;
}

std::vector<Server>& Connection::getServers(){
	return servers;
}

Request* Connection::getRequest(){
	return request;
}

std::string Connection::getLine() {
	std::string line;

	auto pos = msg_in.find("\r\n");
	if (pos != std::string::npos && pos > max_line_len) {
		throw ErrorException(400);
	}
	if (pos != std::string::npos) {
		line = msg_in.substr(0, pos);
		msg_in = msg_in.erase(0, pos + 2);
	}
	return line;
}

void Connection::getRawData(std::string& content, int len) {
	content = msg_in.substr(0, len);
	msg_in.erase(0, len);
}

/* ================================= Setters ================================ */

void Connection::setError(bool er) {
	error = er;
}

void Connection::setMessageIn(std::string msg) {
	msg_in = msg;
}

void Connection::setMessageOut(std::string msg) {
	msg_out = msg;
}

void Connection::setKeepAlive(bool keep) {
	keep_alive = keep;
}

void Connection::setServers(std::vector<Server> serv){
	for (auto s : serv){
		if (s.getListenPort() == listening_port){
			servers.push_back(s);
		}
	}
	setDefaultServer();
}

void  Connection::setDefaultServer() {
	for (auto& s: servers){
		if (s.getListenPort() == listening_port){
			default_server =  &s;
			return;
		}
	}

	throw(ServerException());
}

/* =================================== is Methods =========================== */

bool Connection::isListening() const {
	return listening;
}

/* ================================ Other Methods =========================== */

void Connection::createRequest(){
	deleteRequest();
	request = new Request(this);
}

void Connection::deleteRequest() {
	if (request != nullptr) {
		delete request;
		request = nullptr;
	}
}

void Connection::addRawData(std::string& content, int len) {
	content += msg_in.substr(0, len);
	msg_in.erase(0, len);
}

Connection* Connection::accept() const {
	Connection* connection = new Connection();
	soc.accept(connection->soc);
	connection->listening_port = getPort();
	return connection;
}

int Connection::readAll() {
	return soc.readAll(msg_in);
}

int Connection::send() {
	return soc.send(msg_out);	
}

void Connection::close() {
	soc.close();	
}

/* =============================== STATIC CONST ============================= */
// max length to which the queue of pending connections may grow
constexpr size_t Connection::max_line_len = 20000;
