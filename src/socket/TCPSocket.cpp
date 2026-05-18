#include <cstring>

#include "TCPSocket.hpp"
#include "ErrorException.hpp"
#include "ServerException.hpp"
#include "Request.hpp"

/* ========================================= Coplien Methods ================================= */
// Used for listening sockets
TCPSocket::TCPSocket(int p) : req(nullptr), listening_port(p), keep_alive(true), error(false), listening(true), default_server(nullptr) {
	socket_addr_length = sizeof(socket_addr);

	memset(&socket_addr, 0, socket_addr_length);
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(p);
	socket_addr.sin_addr.s_addr = INADDR_ANY;

	socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (socket_fd == -1)
		throw(SocketException());

	int value = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));

	if (bind(socket_fd, reinterpret_cast<struct sockaddr*>(&socket_addr), sizeof(socket_addr)) ==
		-1)
		throw(SocketException());

	if (listen(socket_fd, backlog) == -1)
	{
		throw(SocketException());
	}
	std::cout << "TCP socket " << socket_fd << " on port " << getPort() << " created\n";
}

// Used for non listening sockets
TCPSocket::TCPSocket()
	: req(nullptr), socket_fd(0), listening_port(0), keep_alive(false), error(false), listening(false){
	socket_addr_length = sizeof(socket_addr);
	memset(&socket_addr, 0, socket_addr_length);
}

TCPSocket::TCPSocket(TCPSocket const& rhs) {
	*this = rhs;
}

TCPSocket::~TCPSocket() {
	if (socket_fd){
		::close(socket_fd);
	}
	if (req) {
		delete req;
		req = nullptr;
	}
}

TCPSocket& TCPSocket::operator=(TCPSocket const& rhs) {
	if (this != &rhs) {
		socket_fd = rhs.socket_fd;
		socket_addr = rhs.socket_addr;
		listening_port = rhs.listening_port;
		socket_addr_length = rhs.socket_addr_length;
		msg_in = rhs.msg_in;
		msg_out = rhs.msg_out;
		req = rhs.req;
		keep_alive = rhs.keep_alive;
		error = rhs.error;
		listening = rhs.listening;
		servers = rhs.servers;
		default_server= rhs.default_server;
	}
	return *this;
}

/* ====================================== Getters ===================================== */

int TCPSocket::getPort() const {
	return ntohs(socket_addr.sin_port);
}

int TCPSocket::getListeningSocketPort() const {
	return listening_port;
}

int TCPSocket::getFd() const {
	return socket_fd;
}

bool TCPSocket::getError() const {
	return error;
}

std::string TCPSocket::getMessageIn() const {
	return msg_in;
}

std::string TCPSocket::getMessageOut() const {
	return msg_out;
}

bool TCPSocket::getKeepAlive() const {
	return keep_alive;
}

bool TCPSocket::getListening() const {
	return listening;
}

Server* TCPSocket::getDefaultServer() const{
	return default_server;
}

std::vector<Server>& TCPSocket::getServers(){
	return servers;
}

std::string TCPSocket::getLine() {
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

void TCPSocket::getRawData(std::string& content, int len) {
	content = msg_in.substr(0, len);
	msg_in.erase(0, len);
}

/* ====================================== Setters ===================================== */

void TCPSocket::setError(bool er) {
	error = er;
}

void TCPSocket::setMessageIn(std::string msg) {
	msg_in = msg;
}

void TCPSocket::setMessageOut(std::string msg) {
	msg_out = msg;
}

void TCPSocket::setKeepAlive(bool keep) {
	keep_alive = keep;
}

void TCPSocket::setServers(std::vector<Server> serv){
	for (auto s : serv){
		if (s.getListenPort() == listening_port){
			servers.push_back(s);
		}
	}
	setDefaultServer();
}


/* ==================================== Other Methods ================================= */
void  TCPSocket::setDefaultServer() {
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

TCPSocket* TCPSocket::accept() const {
	TCPSocket* connection_socket = new TCPSocket();
	connection_socket->socket_fd = ::accept(socket_fd, reinterpret_cast<struct sockaddr*>(&connection_socket->socket_addr),
							   &connection_socket->socket_addr_length);
	if (connection_socket->socket_fd == -1)
		throw(ErrorException(500));
	connection_socket->listening_port = getPort();
	return connection_socket;
}

void TCPSocket::close() {
	if (socket_fd) {
		::close(socket_fd);
		socket_fd = 0;
	}
}

int TCPSocket::readAll() {
	char* buffer = new char[buffer_size + 1];
	int	  read_size = ::read(socket_fd, buffer, buffer_size);
	if (read_size >= 0)
		buffer[read_size] = '\0';
	else{
		throw(SocketException());
	}

	msg_in.insert(0, buffer, static_cast<size_t>(read_size));
	delete[] buffer;
	return read_size;
}

void TCPSocket::addRawData(std::string& content, int len) {
	content += msg_in.substr(0, len);
	msg_in.erase(0, len);
}

void TCPSocket::deleteRequest() {
	if (req != nullptr) {
		delete req;
		req = nullptr;
	}
}

int TCPSocket::send() {
	int len = ::send(socket_fd, msg_out.c_str(), msg_out.length(), 0);
	if (len <= 0){
		throw(SocketException());
	}
	msg_out.erase(0, len);
	return len;
}

/* ============================================ STATIC CONST =============================== */
// max length to which the queue of pending connections may grow
constexpr int	 TCPSocket::backlog = 42;
constexpr size_t TCPSocket::buffer_size = 220000;
constexpr size_t TCPSocket::max_line_len = 20000;
