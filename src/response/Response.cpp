/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 14:49:31 by pjay              #+#    #+#             */
/*   Updated: 2023/09/12 14:30:21 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "TCPSocket.hpp"

std::string Response::getResponse()
{
	std::string response = "HTTP/1.1 " + _status + " \r\n";
	if (!_location.empty())
	{
		response += "Location: " + _location + "\r\n";
		return response;
	}
	if (!_connectionClose.empty())
		response += "Connection: " + _connectionClose + "\r\n";
	response += "Content-Type: " + _contentType + "\r\n" + "Content-Length: " + _contentLength + "\r\n";
	int	cookie_nb = _setCookie.size();
	for (int i = 0 ; i < cookie_nb; i++)
	{
		response += "Set-Cookie: " + _setCookie[i] + "\r\n";
	}
	response += "\r\n";
	response += _content;
	return (response);
}

Response::Response(Request& req, Server& serv)
{
	//std::cout << std::endl << "IN RESPONSE CONSTRUCTOR" << std::endl;
	_readFileAccess = OK;
	_serv = serv;
	std::cout << serv.getDefaultPage()[0] << std::endl;
	_root = _serv.getRoot();
	_autoIndex = _serv.getAutoIndex();
	_allowedMethods = serv.getAllowMethods();
	if (req.getCgiStatus() == 4)
	{
		if (respWithCgi(req, *this) == 0)
			return;
	}
	if (checkIfLocation(req.getQuery(), *this) != -1)
	{
		//std::cout << "EEEEEEEEEEEEEEE" << std::endl;
		if (respWithLoc(req, *this) == 0)
			return ;
	}
	else
	{
		if (respWithOutLoc(req, *this) == 0)
			return ;
	}
	if (req.getMethod() == "GET" && (_allowedMethods == GET || _allowedMethods == GETPOST || _allowedMethods == GETDELETE || _allowedMethods == GETPOSTDELETE))
		dealWithGet(req, *this);
	else if (req.getMethod() == "POST" && (_allowedMethods == POST || _allowedMethods == GETPOST || _allowedMethods == POSTDELETE || _allowedMethods == GETPOSTDELETE))
		dealWithPost(req, *this);
	else if (req.getMethod() == "DELETE" && (_allowedMethods == DELETE || _allowedMethods == GETDELETE || _allowedMethods == POSTDELETE || _allowedMethods == GETPOSTDELETE))
		dealWithDelete(req, *this);
	else
		*this = createErrorPage(405, _serv);
}

Response::Response(std::string status, std::string contentType, std::string contentLength, std::string connectionClose, std::string content)
{
	_status = status;
	_contentType = contentType;
	_contentLength = contentLength;
	_connectionClose = connectionClose;
	_content = content;
}

Response& Response::operator=(Response const & rhs)
{
	_status = rhs._status;
	_contentType = rhs._contentType;
	_contentLength = rhs._contentLength;
	_connectionClose = rhs._connectionClose;
	_content = rhs._content;
	return *this;
}

void Response::Setserv(Server serv)
{
	_serv = serv;
}

void Response::setRoot(std::string root)
{
	_root = root;
}

void Response::setAutoIndex(std::string autoIndex)
{
	_autoIndex = autoIndex;
}

void Response::setAllowedMethods(int allowedMethods)
{
	_allowedMethods = allowedMethods;
}

void Response::setLocation(std::string location)
{
	_location = location;
}

void Response::setStatus(std::string status)
{
	_status = status;
}

void Response::setMethod(std::string method)
{
	_method = method;
}

void Response::setContentType(std::string contentType)
{
	_contentType = contentType;
}

void Response::setContentLength(std::string contentLength)
{
	_contentLength = contentLength;
}

void Response::setConnectionClose(std::string connectionClose)
{
	_connectionClose = connectionClose;
}

void Response::setContent(std::string content)
{
	_content = content;
}

void Response::setExtensionAllowed(std::pair<std::string, std::string> extensionAllowed)
{
	_extensionAllowed = extensionAllowed;
}

void Response::setReadFileAccess(int readFileAccess)
{
	_readFileAccess = readFileAccess;
}

void Response::setContentMap(ContentMap contentMap)
{
	_contentMap = contentMap;
}

Server Response::getServ(void) const
{
	return (_serv);
}

std::string Response::getContentType(void) const
{
	return (_contentType);
}

std::string Response::getContentLength(void) const
{
	return (_contentLength);
}

std::string Response::getStatus(void) const
{
	return (_status);
}

std::string Response::getMethod(void) const
{
	return (_method);
}

std::string Response::getConnectionClose(void) const
{
	return (_connectionClose);
}

std::string Response::getContent(void) const
{
	return (_content);
}

std::string Response::getLocation(void) const
{
	return (_location);
}

std::string Response::getRoot(void) const
{
	return (_root);
}

std::string Response::getAutoIndex(void) const
{
	return (_autoIndex);
}

std::pair<std::string, std::string> Response::getExtensionAllowed(void) const
{
	return (_extensionAllowed);
}

int Response::getReadFileAccess(void) const
{
	return (_readFileAccess);
}

int Response::getAllowedMethods(void) const
{
	return (_allowedMethods);
}

std::vector<std::string> Response::getCookie(void) const
{
	return (_setCookie);
}

ContentMap Response::getContentMap(void) const
{
	return (_contentMap);
}





