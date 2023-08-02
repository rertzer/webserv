/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:15:31 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/02 18:17:01 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(int p, std::string msg):port(p), status(400)
{
	//control data
	int c = msg.find("\r\n");
	if (c == -1)
		return;
	std::string cdata = msg.substr(0, c);
	//headers
int	h = msg.find("\r\n\r\n", c + 1);
	if (h == -1)
		return;
	std::string head = msg.substr(c, h);
	//content
	std::string cont = msg.substr(h);
	//trailer
	status = 200;
	std::cout << "Control data:\n" << cdata << "\nHeaders: " << head << "\nContent: " << cont << std::endl;
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


