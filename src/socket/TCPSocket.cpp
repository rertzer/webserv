/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPSocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 11:28:31 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/02 16:40:55 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TCPSocket.hpp"

// PUBLIC
TCPSocket::TCPSocket(int p)
{
	buffer[0] = '\0';
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(p);
	socket_addr.sin_addr.s_addr = INADDR_ANY;
	
	socket_addr_length = sizeof(socket_addr);

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

TCPSocket::TCPSocket():socket_fd(0)
{
	std::cout << "Default TCPSocket constructor fd " << getFd() << std::endl;
	buffer[0] = '\0';
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
	{
		::close(socket_fd);
		socket_fd = 0;
	}
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

int	TCPSocket::getFd() const
{
	return socket_fd;
}

void	TCPSocket::accept(TCPSocket & csoc)
{
	csoc.socket_fd = ::accept(socket_fd, reinterpret_cast<struct sockaddr*>(&csoc.socket_addr), &csoc.socket_addr_length);
	if (csoc.socket_fd == -1)
		throw(SocketException());
	std::cout << "New connection " << csoc.socket_fd << " from TCP socket " << socket_fd << " on port " << getPort() << std::endl;
}

void	TCPSocket::close()
{
	if (socket_fd)
		::close(socket_fd);
	socket_fd = 0;
}

int	TCPSocket::read()
{
	int	read_size = ::read(socket_fd, buffer, buffer_size);
	msg_in = buffer;
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
