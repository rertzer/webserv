/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPSocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 11:28:31 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/07 14:10:58 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TCPSocket.hpp"

// PUBLIC
TCPSocket::TCPSocket(int p): req(NULL), mother_port(p), keep_alive(false)
{
	socket_addr_length = sizeof(socket_addr);

	memset(&socket_addr, 0, socket_addr_length);
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(p);
	socket_addr.sin_addr.s_addr = INADDR_ANY;


	socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (socket_fd == -1)
		throw(SocketException());

	int	value = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));

	setParam();

	if (bind(socket_fd, reinterpret_cast<struct sockaddr*>(&socket_addr), sizeof(socket_addr)) == -1)
		throw(SocketException());

	if (listen(socket_fd, backlog) == -1)
		throw(SocketException());
	std::cout << "TCP socket " << socket_fd << " on port " << getPort() << " created\n";
}

TCPSocket::TCPSocket(): req(NULL), socket_fd(0), mother_port(0), keep_alive(false)
{
	socket_addr_length = sizeof(socket_addr);
	memset(&socket_addr, 0, socket_addr_length);
}

TCPSocket::TCPSocket(TCPSocket const &rhs)
{
	*this = rhs;
}

TCPSocket::~TCPSocket()
{
	if (socket_fd)
		::close(socket_fd);
	if (req)
	{
		delete req;
		req = NULL;
	}
}

TCPSocket & TCPSocket::operator=(TCPSocket const & rhs)
{
	if (this != &rhs)
	{
		socket_fd = rhs.socket_fd;
		socket_addr = rhs.socket_addr;
		mother_port = rhs.mother_port;
		socket_addr_length = rhs.socket_addr_length;
		msg_in = rhs.msg_in;
		msg_out = rhs.msg_out;
		req = rhs.req;
		keep_alive = rhs.keep_alive;
	}
	return *this;
}

int	TCPSocket::getPort() const
{
	return ntohs(socket_addr.sin_port);
}

int	TCPSocket::getMotherPort() const
{
	return mother_port;
}

int	TCPSocket::getFd() const
{
	return socket_fd;
}


void	TCPSocket::accept(TCPSocket * csoc)
{
	csoc->socket_fd = ::accept(socket_fd, reinterpret_cast<struct sockaddr*>(&csoc->socket_addr), &csoc->socket_addr_length);
	if (csoc->socket_fd == -1)
		throw(ErrorException(500));
	csoc->mother_port = getPort();
	csoc->setParam();
	std::cout << "New connection " << csoc->socket_fd <<  ":" << csoc->getPort() << " from TCP socket " << socket_fd << " on port " << getPort() << std::endl;
}

void	TCPSocket::setParam()
{
	int optval = 200000;
	socklen_t optlen = sizeof(optval);
	if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &optval, optlen) == -1)
	getsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &optval, &optlen);
}

void	TCPSocket::close()
{
	if (socket_fd)
	{
		::close(socket_fd);
		socket_fd = 0;
	}
}

int	TCPSocket::readAll()
{
	char * buffer = new char[buffer_size + 1];
	int	read_size = ::read(socket_fd, buffer, buffer_size);
	if (read_size >= 0)
		buffer[read_size] = '\0';
	else
		throw (SocketException());
	std::stringstream	ss;
	ss.write(buffer, read_size);
	delete[] buffer;
	msg_in = ss.str();
	return read_size;
}


std::string	TCPSocket::getMessageIn() const
{
	return msg_in;
}

std::string TCPSocket::getMessageOut() const
{
	return msg_out;
}

void	TCPSocket::setMessageIn(std::string msg)
{
	msg_in = msg;
}

void	TCPSocket::setMessageOut(std::string msg)
{
	msg_out = msg;
}

std::string	TCPSocket::getLine()
{
	int	pos = -1;
	std::string	line;

	pos = msg_in.find("\r\n");
	if (pos != -1)
	{
		line = msg_in.substr(0, pos);
		msg_in = msg_in.erase(0, pos + 2);
	}
	return (line);
}

void	TCPSocket::getRawData(std::string & content, int len)
{
	content = msg_in.substr(0, len);
	msg_in.erase(0, len);
}

void	TCPSocket::addRawData(std::string & content, int len)
{
	content += msg_in.substr(0, len);
	msg_in.erase(0, len);
}

bool	TCPSocket::getKeepAlive() const
{
	return keep_alive;
}

void	TCPSocket::setKeepAlive(bool k)
{
	keep_alive = k;
}

int	TCPSocket::send()
{
	int len = ::send(socket_fd, msg_out.c_str(), msg_out.length(), 0);
	msg_out.erase(0, len);
	return len;
}

// STATIC CONST
//max length to which the queue of pending connections may grow
const int	TCPSocket::backlog = 42;
//read buffer size
const int	TCPSocket::buffer_size = 220000;
