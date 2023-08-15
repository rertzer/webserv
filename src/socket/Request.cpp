/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:15:31 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/15 10:58:41 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(TCPSocket * s):port(s->getMotherPort()), status(100), soc(s)
{
	std::cout << "fd " << soc->getFd() << " is reading\n";
	soc->readAll();
	setControlData();
	setHeader();
	if (contentExist())
		setContent();
	
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
		content = rhs.content;
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

bool	Request::checkField(std::string const & name, std::string const & value) const
{
	std::string field = getField(name);
	std::vector<std::string> all_values = splitCsv(field);
	for (size_t i = 0; i < all_values.size(); i++)
	{
		if (ciCompare(all_values[i], value))
			return true;
	}
	return false;
}	

int	Request::getIntField(std::string const & name) const
{
	std::stringstream ss;
	ss << getField(name);
	int val = 0;
	ss >> val;
	return val;
}

const std::map<std::string, std::string> &	Request::getHeader() const
{
	return header;
}

const std::map<std::string, std::string> &	Request::getTrailer() const
{
	return trailer;
}

const std::string &	Request::getContent() const
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
void	Request::setControlData()
{
	std::string line = soc->getLine();

	if (line.empty())
		line = soc->getLine();

	int	m = line.find(" ");
	if (m == -1)
		throw (ErrorException(400));
	method = line.substr(0, m);

	int	q = line.find(" ", m + 1);
	if (q == -1)
		throw (ErrorException(400));
	query = line.substr(m + 1, q - (m + 1));
	
	protocol = line.substr(q + 1);
	
	checkControlData();
}

void	Request::setHeader()
{
	int	line_nb = 1;

	std::string	line = soc->getLine();

	while (line.length())
	{
		if (line.length())
		{
			addField(line);
			line_nb++;
		}
		line = soc->getLine();
	}
}

void	Request::setContent()
{
	std::cout << "setContent\n";
	std::string trans_encoding = getField("Transfer-Encoding");

	if (! trans_encoding.empty())
	{
		if (checkField("Transfer-Encoding", "chunked"))
			setContentByChunked();
		else
			throw (ErrorException(501));
	}
	else
	{
		int len = getIntField("Content-Length");
		
		if (len > 0 && len < 16777216)
			setContentByLength(len);
		else
			throw (ErrorException(400));
	}
}

void	Request::setContentByChunked()
{
	std::cout << "setContentByChunked\n";
}


void	Request::setContentByLength(int len)
{
	std::cout << "setContentByLength\n";
	soc->getRawData(content, len);
}

void	Request::checkControlData() const
{
	if (protocol != "HTTP/1.1")
		throw (ErrorException(505));
	std::vector<std::string> allowed_methods;

	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
	allowed_methods.push_back("DELETE");

	for (unsigned int i = 0; i < allowed_methods.size(); i++)
	{
		if (method == allowed_methods[i])
			return;
	}
	throw (ErrorException(501));
}

void	Request::checkHeader() const
{
	if (getField("Host").empty())
		throw (ErrorException(400));
}

bool	Request::contentExist() const
{
	int	content = 0;
	if (! getField("Transfer-Encoding").empty())
		content++;
	if (! getField("Content-Length").empty())
		content++;
	if (content == 2)
		throw (ErrorException(400));
	if (content)
		return true;
	return false;
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
