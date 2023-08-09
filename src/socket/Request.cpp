/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:15:31 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/09 13:17:28 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(int p, std::string msg):port(p), status(200)
{
	setControlData(msg);
	
	std::cout << "Request created:\n" << *this << std::endl;
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

const std::string &	Request::getMethod() const
{
	return method;
}

const std::string &	Request::getQuery() const
{
	return query;
}

std::string	Request::getField(std::string const & name) const
{
	std::map<std::string, std::string>::const_iterator it = header.find(name);
	if (it == header.end())
	{
		it = trailer.find(name);
		if (it == trailer.end())
			return "";
	}
	return it->second;
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

void	Request::addField(std::string const & field)
{
	int	k = field.find(":");
	if (k == -1 || k == 0)
		throw (ErrorException(400));
	std::string	key = field.substr(0, k);
	std::string	val = field.substr(k + 1);
	stringTrim(val);
	stringTrim(key);
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
		throw (ErrorException(400));
	method = cdata.substr(0, m);

	int	q = cdata.find(" ", m + 1);
	if (q == -1)
		throw (ErrorException(400));
	query = cdata.substr(m + 1, q - (m + 1));
	protocol = cdata.substr(q + 1);
	void checkControlData();
}

void	Request::checkControlData() const
{
	if (protocol != "HTTP/1.0")
		throw (ErrorException(505));
	std::vector<std::string> allowed_methods;

	allowed_methods.push_back("GET");
	allowed_methods.push_back("HEAD");
	allowed_methods.push_back("POST");
	allowed_methods.push_back("DELETE");

	for (unsigned int i = 0; i < allowed_methods.size(); i++)
	{
		if (method == allowed_methods[i])
			return;
	}
	throw (ErrorException(501));
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
	ost << "content: " << rhs.getContent().str() << "\n";
	return ost;
}
