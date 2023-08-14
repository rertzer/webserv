/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPSocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 11:28:31 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/14 15:30:29 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TCPSocket.hpp"

// PUBLIC
TCPSocket::TCPSocket(int p):mother_port(p)
{
	memset(&buffer, 0, 1025);
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
	if (bind(socket_fd, reinterpret_cast<struct sockaddr*>(&socket_addr), sizeof(socket_addr)) == -1)
		throw(SocketException());

	if (listen(socket_fd, backlog) == -1)
		throw(SocketException());
	std::cout << "TCP socket " << socket_fd << " on port " << getPort() << " successfully created\n";
}

TCPSocket::TCPSocket():socket_fd(0), mother_port(0)
{
	std::cout << "Default TCPSocket constructor fd " << getFd() << std::endl;

	socket_addr_length = sizeof(socket_addr);
	memset(&socket_addr, 0, socket_addr_length);
	memset(&buffer, 0, 1025);
}

TCPSocket::TCPSocket(TCPSocket const &rhs)
{
	std::cout << "copy constructor fd " << rhs.getFd() << std::endl;
	*this = rhs;
}


TCPSocket::~TCPSocket()
{
	std::cout << "Default TCPSocket destructor fd " << getFd() << std::endl;
	if (socket_fd)
		::close(socket_fd);
}

TCPSocket & TCPSocket::operator=(TCPSocket const & rhs)
{
	std::cout << "assign op fd " << rhs.getFd() << std::endl;
	if (this != &rhs)
	{

		socket_fd = rhs.socket_fd;
		socket_addr = rhs.socket_addr;
		socket_addr_length = rhs.socket_addr_length;
		msg_in = rhs.msg_in;
		msg_out = rhs.msg_out;
		for (int i = 0; i < 1024; i++)
			buffer[i] = rhs.buffer[i];
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
	std::cout << "New connection " << csoc->socket_fd <<  ":" << csoc->getPort() << " from TCP socket " << socket_fd << " on port " << getPort() << "addr_len " << csoc->socket_addr_length << std::endl;
}

void	TCPSocket::close()
{
	if (socket_fd)
	{
		::close(socket_fd);
		socket_fd = 0;
	}
}

int	TCPSocket::read()
{
	int	read_size = ::read(socket_fd, buffer, buffer_size);
	if (read_size > 0)
		buffer[read_size] = '\0';
	return read_size;
}

int	TCPSocket::rawRead(std::stringstream & content, int len)
{
	int	total_read = 0;

    content << msg_in;
	len -= msg_in.length();
	msg_in.clear();

	while (len > 0)
	{
		std::cout << "len to read: " << len << ". Total read: " << total_read << std::endl;
		int to_read = (len < buffer_size) ? len : buffer_size;
		// centraliser les read
		int	read_size = ::read(socket_fd, buffer, to_read);
		if (read_size < 0 || (read_size == 0 && len))
			throw (ErrorException(500));
		content.write(buffer, read_size);
		len -= read_size;
		total_read += read_size;
	}
	return total_read;
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

std::string	TCPSocket::readLine()
{
	int	pos = -1;
	std::string	line;

	while (1)
	{
		pos = msg_in.find("\r\n");
		if (pos != -1)
		{
			line = msg_in.substr(0, pos);
			msg_in = msg_in.erase(0, pos + 2);
			//std::cout << "read line " << line << std::endl;
			break;
		}

		int len = read();

		if (len == -1)
			throw (SocketException());
		if (len == 0)
			throw (SocketException());
		if (msg_in.length() + static_cast<unsigned int>(len) >= line_size_max)
			throw (SocketException());

		msg_in += buffer;
	}
	return line;
}

int	TCPSocket::send()
{
	int len = ::send(socket_fd, msg_out.c_str(), msg_out.length(), 0);
	msg_out.clear();
	return len;
}

// Private
// STATIC CONST
//max length to which the queue of pending connections may grow
const int	TCPSocket::backlog = 42;
//read buffer size
const int	TCPSocket::buffer_size = 1024;
const unsigned int	TCPSocket::line_size_max = 20000;
