/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:15:31 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/03 13:48:10 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(int p, std::string msg):port(p), status(200)
{
	try
	{
		//control data
		int c = msg.find("\r\n");
		if (c == -1)
			throw (RequestException());
		setControlData(msg.substr(0, c));
		
		//headers
		int	h = msg.find("\r\n\r\n", c + 2);
		if (h == -1)
			throw (RequestException());
		std::string head = msg.substr(c + 2, h - (c + 2));
		setHeader(head);
		
		//content
		std::string cont = msg.substr(h + 4);
		//content << cont;	
		//trailer
	}
	catch (const Request::RequestException &e)
	{
		std::cerr << e.what() << std::endl;
		status = 400;
	}
	std::cout << "Request created:\n" << *this << "Processing..." << std::endl;
}

Request::Request(Request const & rhs)
{
	*this = rhs;
}

Request::~Request()
{}

Request &	Request::operator=(Request const & rhs)
{
	if (this != &rhs)
	{
		port = rhs.port;
		status = rhs.status;
		header = rhs.header;
		trailer = rhs.trailer;
		query = rhs.query;
		method = rhs.method;
		content << rhs.content;
	}
	return *this;
}

int	Request::getPort() const
{
	return port;
}

int	Request::getStatus() const
{
	return status;
}

const std::string	& Request::getProtocol() const
{
	return protocol;
}

const std::string	& Request::getMethod() const
{
	return method;
}

const std::string	 & Request::getQuery() const
{
	return query;
}

const std::map<std::string, std::string> &	Request::getHeader() const
{
	return header;
}

const std::map<std::string, std::string> &	Request::getTrailer() const
{
	return trailer;
}

const std::stringstream &	Request::getContent() const
{
	return content;
}

void	Request::addField(std::string field)
{
	int	k = field.find(":");
	if (k == -1 || k == 0)
		throw (RequestException());
	std::string	key = field.substr(0, k);
	std::string	val = field.substr(k + 1);
	// A FAIRE tester pas d'esapce avant le :
	if (header.find(key) == header.end())
		header[key] = val;
	else
		header[key] += ", " + val;
}

// Private
void	Request::setControlData(std::string cdata)
{
	int	m = cdata.find(" ");
	if (m == -1)
		throw (RequestException());
	method = cdata.substr(0, m);

	int	q = cdata.find(" ", m + 1);
	if (q == -1)
		throw (RequestException());
	query = cdata.substr(m + 1, q - (m + 1));
	protocol = cdata.substr(q + 1);
}

void	Request::setHeader(std::string head)
{
	int	start = 0;
	int	end = -1;

	while (start != -1)
	{
		end = head.find("\r\n", start);
		std::string	field = head.substr(start, end - (start));
		addField(field);
		if (end == -1)
			start = end;
		else
			start = end + 2;
	}
}

//

std::ostream &	operator<<(std::ostream & ost, Request const & rhs)
{
	ost << "port: " << rhs.getPort() << "\n";
	ost << "protocol: " << rhs.getProtocol() << "\n";
	ost << "method: " << rhs.getMethod() << "\n";
	ost << "query: " << rhs.getQuery() << "\n";
	ost << "status: " << rhs.getStatus() << "\n";
	ost << "Header:\n";
	for (std::map<std::string, std::string>::const_iterator it = rhs.getHeader().begin(); it != rhs.getHeader().end(); it++)
		ost << "\t" << it->first << ": " << it->second << "\n";
	ost << "Trailer:\n";
	for (std::map<std::string, std::string>::const_iterator it = rhs.getTrailer().begin(); it != rhs.getTrailer().end(); it++)
		ost << "\t" << it->first << ": " << it->second << "\n";
	ost << "content: " << rhs.getContent() << "\n";
	return ost;
}
