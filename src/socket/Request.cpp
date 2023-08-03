/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:15:31 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/03 10:08:18 by rertzer          ###   ########.fr       */
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
		int	h = msg.find("\r\n\r\n", c + 1);
		if (h == -1)
			throw (RequestException());
		std::string head = msg.substr(c + 1, h - c - 1);
		//content
		std::string cont = msg.substr(h);
		//trailer
		std::cout << "method: $" << method << "$\nquery: $" << query << "$\nprotocol: $" << protocol << "$" <<std::endl;
		std::cout << "\nHeaders: " << head << "\nContent: " << cont << std::endl;
	}
	catch (const Request::RequestException &e)
	{
		std::cerr << e.what() << std::endl;
		status = 400;
	}
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

void	Request::setControlData(std::string cdata)
{
	int	m = cdata.find(" ");
	if (m == -1)
		throw (RequestException());
	method = cdata.substr(0, m);

	int	q = cdata.find(" ", m + 1);
	if (q == -1)
		throw (RequestException());
	query = cdata.substr(m + 1, q - m - 1);
	protocol = cdata.substr(q + 1);
}
