/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 14:49:31 by pjay              #+#    #+#             */
/*   Updated: 2023/09/12 14:17:01 by pjay             ###   ########.fr       */
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

int Response::checkIfLocation(std::string path)
{
	std::vector<Location> loc = _serv.getAllLocation();
	std::vector<Location>::iterator it = loc.begin();
	//std::cout << "Path before tje substr " << path << std::endl;
	if (path != "/")
		path = path.substr(0, path.rfind("/") );
	while (it != loc.end())
	{
		if (it->getLocationPath() == path)
		{
			return (0);
		}
		it++;
	}
	return (-1);
}

Location Response::getTheLocation(std::string path)
{
	std::vector<Location> loc = _serv.getAllLocation();
	std::vector<Location>::iterator it = loc.begin();
	if (path != "/")
		path = path.substr(0, path.rfind("/") );
	while (it != loc.end())
	{
		if (it->getLocationPath() == path)
		{
			//std::cout << "Check if good location " << it->getLocationPath() << std::endl;
			return (*it);
		}
		it++;
	}
	return (Location());
}

std::string Response::getSpecIndex(Location loc)
{
	std::vector<std::string> index = loc.getIndex();
	if (index.size() == 0)
		return ("");
	else
	{
		for (std::vector<std::string>::iterator it = index.begin() ; it != index.end(); it++)
		{
			//std::cout << "In get spec index = " << (_root + *it) << std::endl;
			if (access((_root + *it).c_str(), F_OK) != -1 && access((_root + *it).c_str(), R_OK) != -1)
			{
				//std::cout << "ALL GOOD ??" << std::endl;
				return (*it);
			}
		}
		return ("");
	}
}

void Response::createAutoIndexResp(Request& req, Location loc) {

	int allowMethod = checkAllowMethod(loc);
	if (allowMethod != -1)
		_allowedMethods = allowMethod;
	std::cout << "allow methode in create auto index =" << allowMethod << " old allow method = " << _allowedMethods<<  std::endl;
	if ((req.getMethod() == "GET" && (_allowedMethods == GET || _allowedMethods == GETPOST || _allowedMethods == GETDELETE || _allowedMethods == GETPOSTDELETE)) || \
		(req.getMethod() == "POST" && (_allowedMethods == POST || _allowedMethods == GETPOST || _allowedMethods == POSTDELETE || _allowedMethods == GETPOSTDELETE)))
	{
		_content = dirContent(_root, req.getQuery());
		_status = "200";
		_method = req.getMethod();
		std::cout << "enmter here " << std::endl;
		_contentType = "text/html";
		_contentLength = intToString(_content.length());
	}
	else
	{
		*this = createErrorPage(405, _serv);
	}

}

void	Response::setCookie(std::string ck)
{
	_setCookie.push_back(ck);
}

int Response::respWithLoc(Request& req)
{
	Location loc = getTheLocation(req.getQuery());
		if (req.getQuery() != "/")
	{
		if (req.getQuery()[req.getQuery().length() - 1] == '/')
		{
			if (checkAutoIndex(loc) == 0)
			{
				if (getSpecIndex(loc) == "")
					req.setQuery("/");
				else
				{
					//std::cout << "Spec index = "<< getSpecIndex(loc) << std::endl;
					req.setQuery(getSpecIndex(loc));
				}
				loc = getTheLocation(req.getQuery());
			}
			else if (checkAutoIndex(loc) == 1 || _autoIndex == "on")
			{
				createAutoIndexResp(req, loc);
				return (0);
			}
			else
				req.setQuery("/");
		}
	}
	int allowMethod = checkAllowMethod(loc);
	if (allowMethod != -1)
		_allowedMethods = allowMethod;
	if (isThereAspecRoot(loc) == 1)
	{
		//std::cout << "got a new Root WQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ" << std::endl;
		_root = getArgsLoc(loc, "root");
	}
	if (checkForRedirection(loc) == 1)
	{
		//std::cout << "ENter in the redir zone" << std::endl;
		std::pair<std::string, std::string> redirection = RedirectTo(loc);
		_status = Status::getMsg(atoi((redirection.first.c_str())));
		_location = redirection.second;
		return (0);
	}
	//std::cout << "extension = " << getExtension(loc).first << " | Exec with " << getExtension(loc).second << std::endl;
	if (!getExtension(loc).first.empty() && req.getQuery().find(getExtension(loc).first) != std::string::npos)
	{
		return initCgi(req, loc);
	}
	else
	{
		req.setUploadPath(getUploadPath(loc));
		if (req.isUpload())
		{
			req.upload_all();
		}
	}
	return (1);
}

int	Response::initCgi(Request & req, Location & loc)
{
	Cgi *	myCgi = new Cgi(req.getMethod(), _root, req, getExtension(loc));
	req.setCgi(myCgi);
	return (0);
}

int	Response::respWithCgi(Request & req)
{
	std::cout << "RespWithCgi==========================================================================\n";
	_method = req.getMethod();
	_content = req.getCgi()->getContent();
	_contentType = "text/html";

	size_t pos = _content.find("\r\n");
	while (pos != std::string::npos && pos != 0)
	{
		std::pair<std::string, std::string> field = extractField(pos);
		if (field.first == "Content-Type")
		{
			_contentType = field.second;
		}
		else if (field.first == "Set-Cookie")
		{
			_setCookie.push_back(field.second);
		}
		pos = _content.find("\r\n");
	}
	_status = "200 OK";
	_contentLength = intToString(_content.length());
	_connectionClose = "keep-alive";
	return (0);
}
/*
std::string	Response::runFile(Request & req)
{

	if (access(req.getCgi()->getPath().c_str(), F_OK) == -1)
	{
		_readFileAccess = FILE_NOT_FOUND;
		return ("404");
	}
	if (access(req.getCgi()->getPath().c_str(), R_OK) == -1)
	{
		_readFileAccess = ACCESS_DENIED;
		return ("403");
	}
	_readFileAccess = OK;
	req.getCgi()->exec();
	return (req.getCgi()->getContent());
}
*/
std::pair<std::string, std::string>	Response::extractField(size_t pos)
{
	std::string	line;
	std::pair<std::string, std::string>	field;

	if (pos != std::string::npos)
	{
		line = _content.substr(0, pos);
		_content.erase(0, pos + 2);
		pos = line.find(":");
		if (pos != std::string::npos)
		{
			field.first = line.substr(0, pos);
			field.second = line.substr(pos + 1, -1);
		}
		stringTrim(field.first);
		stringTrim(field.second);
	}
	return (field);
}

int Response::respWithOutLoc(Request& req)
{
	if (req.getQuery()!= "/")
	{
		if (req.getQuery()[req.getQuery().length() - 1] == '/')
		{
			std::cout << "Enter here in resp withou loc }} autouindex = " << _autoIndex << std::endl;
			if (_autoIndex == "on")
			{
				std::cout << "Enter here in resp withou loc" << std::endl;
				createAutoIndexResp(req, Location());
				return (0);
			}
			else
				req.setQuery("/");
		}
	}
	return (1);
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





