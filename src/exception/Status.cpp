/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Status.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/07 11:41:50 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/07 11:59:00 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Status.hpp"

std::string	Status::getReason(int code)
{
	std::map<int, std::string>	reason;

	reason[100] = "Continue";
	reason[200] = "OK";
	reason[201] = "Created";
	reason[202] = "Accepted";
	reason[204] = "No Content";
	reason[301] = "Moved Permanently";
	reason[302] = "Found";
	reason[303] = "See Other";
	reason[304] = "Not Modified";
	reason[307] = "Temporary Redirect";
	reason[308] = "Permanent Redirect";
	reason[400] = "Bad Request";
	reason[403] = "Forbidden";
	reason[404] = "Not Found";
	reason[405] = "Method Not Allowed";
	reason[406] = "Not Acceptable";
	reason[408] = "Request Timeout";
	reason[417] = "Expectation Failed";
	reason[418] = "I'm a teapot";
	reason[451] = "Unavailable For Legal Reasons";
	reason[500] = "Internal Server Error";
	reason[501] = "Not Implemented";
	reason[502] = "Bad Gateway";
	reason[503] = "Service Unavailable";
	reason[504] = "Gateway Timeout";
	reason[505] = "HTTP Version Not Supported";
	
	std::map<int, std::string>::const_iterator cit = reason.find(code);
	if (cit == reason.end())
		return "";
	return cit->second;
}

std::string Status::getMsg(int code)
{
	std::stringstream ss;
	ss << code << " " << getReason(code);
	return ss.str();
}
