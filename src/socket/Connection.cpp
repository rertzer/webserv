#include <cstring>

#include "Connection.hpp"
#include "ErrorException.hpp"
#include "ServerException.hpp"
#include "Request.hpp"

/* ========================================= Coplien Methods ================================= */
// Used for listening sockets
Connection::Connection(int p) :req(nullptr), listening_port(p), keep_alive(true), error(false), listening(true), default_server(nullptr),  soc(TcpSocket(p)) {
	}

// Used for non listening sockets
Connection::Connection()
	: req(nullptr), listening_port(0), keep_alive(false), error(false), listening(false), soc(TcpSocket()){
	
}

Connection::Connection(Connection const& rhs) {
	*this = rhs;
}

Connection::~Connection() {
	if (req) {
		delete req;
		req = nullptr;
	}
}

Connection& Connection::operator=(Connection const& rhs) {
	if (this != &rhs) {
	
		listening_port = rhs.listening_port;
		msg_in = rhs.msg_in;
		msg_out = rhs.msg_out;
		req = rhs.req;
		keep_alive = rhs.keep_alive;
		error = rhs.error;
		listening = rhs.listening;
		servers = rhs.servers;
		default_server= rhs.default_server;
		soc = rhs.soc;
	}
	return *this;
}

/* ====================================== Getters ===================================== */

int Connection::getPort() const {
	return soc.getPort();
}

int Connection::getListeningSocketPort() const {
	return listening_port;
}

int Connection::getFd() const {
	return soc.getSocketFd();
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

bool Connection::getListening() const {
	return listening;
}

Server* Connection::getDefaultServer() const{
	return default_server;
}

std::vector<Server>& Connection::getServers(){
	return servers;
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

/* ====================================== Setters ===================================== */

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

/* ==================================== Other Methods ================================= */
void  Connection::setDefaultServer() {
	std::vector<Server>::iterator it = servers.begin();

	while (it != servers.end()) {
		if (it->getListenPort() == listening_port) {
			default_server =  &*it;
			return;
		}
		it++;
	}
	throw(ServerException());
}

Connection* Connection::accept() const {
	Connection* connection_socket = new Connection();
	soc.accept(connection_socket->soc);
	connection_socket->listening_port = getPort();
	return connection_socket;
}

void Connection::close() {
	soc.close();	
}

int Connection::readAll() {
	return soc.readAll(msg_in);
}

int Connection::send() {
	return soc.send(msg_out);	
}

void Connection::addRawData(std::string& content, int len) {
	content += msg_in.substr(0, len);
	msg_in.erase(0, len);
}

void Connection::deleteRequest() {
	if (req != nullptr) {
		delete req;
		req = nullptr;
	}
}

/* ============================================ STATIC CONST =============================== */
// max length to which the queue of pending connections may grow
constexpr size_t Connection::max_line_len = 20000;
