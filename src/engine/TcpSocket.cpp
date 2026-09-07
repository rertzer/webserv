#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include "webserv/ErrorException.hpp"
#include "webserv/TcpSocket.hpp"

TcpSocket::TcpSocket() : socket_fd(0) {
	socket_addr_length = sizeof(socket_addr);
	memset(&socket_addr, 0, socket_addr_length);
}

TcpSocket::TcpSocket(int port) {
	socket_addr_length = sizeof(socket_addr);

	memset(&socket_addr, 0, socket_addr_length);
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(port);
	socket_addr.sin_addr.s_addr = INADDR_ANY;

	socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (socket_fd == -1)
		throw(SocketException());

	int value = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));

	if (bind(socket_fd, reinterpret_cast<struct sockaddr*>(&socket_addr), sizeof(socket_addr)) ==
		-1)
		throw(SocketException());

	if (listen(socket_fd, backlog) == -1) {
		throw(SocketException());
	}
	std::cout << "TCP socket " << socket_fd << " on port " << getPort() << " created\n";
}

TcpSocket::~TcpSocket() {
	if (socket_fd) {
		::close(socket_fd);
	}
}

TcpSocket& TcpSocket::operator=(TcpSocket const& rhs) {
	if (this != &rhs) {
		socket_fd = rhs.socket_fd;
		socket_addr = rhs.socket_addr;
		socket_addr_length = rhs.socket_addr_length;
	}
	return *this;
}

/* ============================= Getters ==================================== */

int TcpSocket::getPort() const {
	return ntohs(socket_addr.sin_port);
}

std::string TcpSocket::getAddress() const {
	char ip_address[INET_ADDRSTRLEN];
	::inet_ntop(AF_INET, &socket_addr.sin_addr, ip_address, sizeof(ip_address));
	return std::string{ip_address};
}

int TcpSocket::getFd() const {
	return socket_fd;
}

/* ============================= Methods ==================================== */

void TcpSocket::accept(TcpSocket& csoc) const {
	csoc.socket_fd = ::accept(socket_fd, reinterpret_cast<struct sockaddr*>(&csoc.socket_addr),
							  &csoc.socket_addr_length);
	if (csoc.socket_fd == -1)
		throw(ErrorException(HttpStatus::INTERNAL_SERVER_ERROR));
	std::cout << "Accepted connection from " << getAddress() << ":" << getPort() << std::endl;
}

void TcpSocket::close() {
	if (socket_fd) {
		::close(socket_fd);
		socket_fd = 0;
	}
}

int TcpSocket::readAll(std::string& msg) const {
	char* buffer = new char[buffer_size + 1];
	int	  read_size = ::read(socket_fd, buffer, buffer_size);
	if (read_size >= 0)
		buffer[read_size] = '\0';
	else {
		throw(SocketException());
	}

	msg.insert(0, buffer, static_cast<size_t>(read_size));
	delete[] buffer;
	return read_size;
}

int TcpSocket::send(std::string& msg) const {
	int len = ::send(socket_fd, msg.c_str(), msg.length(), 0);
	if (len <= 0) {
		throw(SocketException());
	}
	msg.erase(0, len);
	return len;
}
