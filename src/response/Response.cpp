/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 14:49:31 by pjay              #+#    #+#             */
/*   Updated: 2023/08/23 12:11:27 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

void checkExec(std::string filePath)
{
	if (access(filePath.c_str(), F_OK) == -1)
	{
		throw (ErrorException(404));
	}
	if (access(filePath.c_str(), R_OK) == -1)
	{
		throw (ErrorException(403));
	}
}

std::string Response::readFile(std::string file)
{
	std::ifstream fileOp;
	fileOp.open(file.c_str());
	if (access(file.c_str(), F_OK) == -1)
	{
			_readFileAccess = FILE_NOT_FOUND;
			return ("404");
	}
	if (access(file.c_str(), R_OK) == -1)
	{
			_readFileAccess = ACCESS_DENIED;
			return ("403");
	}
	if (fileOp.is_open())
	{
		std::stringstream fileStr;
		fileStr << fileOp.rdbuf();
		fileOp.close();
		_readFileAccess = OK;
		return (fileStr.str());
	}
	else
	{
		fileOp.close();
		_readFileAccess = FILE_NOT_FOUND;
		return ("404");
	}
}

std::string	Response::runFile(std::string method, Request & req)
{
	Cgi	myCgi(method, _serv.getRoot(), req);

	if (access(myCgi.getPath().c_str(), F_OK) == -1)
	{
			_readFileAccess = FILE_NOT_FOUND;
			return ("404");
	}
	if (access(myCgi.getPath().c_str(), R_OK) == -1)
	{
			_readFileAccess = ACCESS_DENIED;
			return ("403");
	}
	_readFileAccess = OK;
	myCgi.exec();
	return (myCgi.getContent());
}


void Response::feelPart(Request req)
{
	//_method = "GET";
	if (req.getQuery() == "/")
	{
		std::string fileStr;
		for (std::vector<std::string>::iterator it = _serv.getDefaultPage().begin(); it != _serv.getDefaultPage().end(); it++)
		{
			fileStr = readFile(_serv.getRoot() + *it);
			if (fileStr == "403" && _readFileAccess == ACCESS_DENIED)
			{
				break;
			}
			if (fileStr != "404" && _readFileAccess != FILE_NOT_FOUND)
			{
				_contentType = _contentMap.getContentValue(it->substr(it->rfind(".") + 1, it->length()));
				break;
			}
		}
		if (fileStr == "404" && _readFileAccess == FILE_NOT_FOUND)
			*this = createErrorPage(404, _serv);
		else if (fileStr == "403" && _readFileAccess == ACCESS_DENIED)
			*this = createErrorPage(403, _serv);
		else
		{
			_status = "200 OK";
			_content = fileStr;
			_contentLength = intToString(_content.length()); // mettre en string
			_connectionClose = "keep-alive";
		}
	}
	else
	{
		std::string	fileStr;
		if (req.getQuery().find(".php") != std::string::npos)
		{
			fileStr= runFile(_method, req);
		}
		else
		{
			std::cout << "_serv.getRoot() + req.getQuery() = " << _serv.getRoot() + req.getQuery() << std::endl;
			std::cout << "req.getQuery() = " << req.getQuery() << std::endl;
			std::cout << "_serv.getRoot() = " << _serv.getRoot() << std::endl;
			fileStr = readFile(_serv.getRoot() + req.getQuery());
			//std::cout << "Content that is not root " << fileStr << std::endl;
		}
		if (fileStr == "404" && _readFileAccess == FILE_NOT_FOUND)
			*this = createErrorPage(404, _serv);
		else if (fileStr == "403" && _readFileAccess == ACCESS_DENIED)
			*this = createErrorPage(403, _serv);
		else
		{
			_status = "200 OK";
			if (req.getQuery().find(".php") != std::string::npos)
			{
				size_t pos = fileStr.find("\r\n\r\n");
				if (pos != std::string::npos)
				{
					_contentType = fileStr.substr(14, pos);
					fileStr.erase(0, pos + 4);
				}
				else
					_contentType = "text/html";
			}
			else
				_contentType = _contentMap.getContentValue(req.getQuery().substr(req.getQuery().rfind(".") + 1, req.getQuery().length()));;
			_content = fileStr;
			_contentLength = intToString(_content.length());
			_connectionClose = "keep-alive";
		}
	}
}

void Response::dealWithGet(Request req)
{
	_method = "GET";
	feelPart(req);

}

void Response::dealWithPost(Request req)
{
	_method = "POST";
	feelPart(req);
}

void Response::dealWithDelete(Request req)
{
	_method = "DELETE";
	checkExec(_serv.getRoot() + req.getQuery());
	if (std::remove((_serv.getRoot() + req.getQuery()).c_str()) != 0)
	{
		*this = createErrorPage(404, _serv);
	}
	else
	{
		_status = "200 OK";
		_contentType = "text/html";
		_content = "<html><body>File deleted</body></html>";
		_contentLength = intToString(_content.length());
	}
}

std::string Response::getResponse()
{
	std::string response = "HTTP/1.1 " + _status + " \r\n";
	if (!_connectionClose.empty())
		response += "Connection: " + _connectionClose + "\r\n";
	response += "Content-Type: " + _contentType + "\r\n"
							"Content-Length: " + _contentLength + "\r\n"
							"\r\n";
	response += _content;
	return (response);
}


int Response::checkIfLocation(std::string path)
{
	std::vector<Location> loc = _serv.getAllLocation();
	std::vector<Location>::iterator it = loc.begin();
	if (path != "/")
		path = path.substr(0, path.rfind("/") );
	while (it != loc.end())
	{
		std::cout << "location path = " << it->getLocationPath() << std::endl;
		std::cout <<  "path = " << path << std::endl;
		if (it->getLocationPath() == path)
		{
			std::cout << "found a location path" << std::endl;
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
			std::cout << "In get spec index = " << (_serv.getRoot() + *it) << std::endl;
			if (access((_serv.getRoot() + *it).c_str(), F_OK) != -1 && access((_serv.getRoot() + *it).c_str(), R_OK) != -1)
			{
				std::cout << "ALL GOOD ??" << std::endl;
				return (*it);
			}
		}
		return ("");
	}
}

void Response::createAutoIndexResp(Request& req, Location loc) {

	int allowMethod = checkAllowMethod(loc);
	if ((req.getMethod() == "GET" && (allowMethod == GET || allowMethod == GETPOST || allowMethod == GETDELETE || allowMethod == GETPOSTDELETE)) || \
		(req.getMethod() == "POST" && (allowMethod == POST || allowMethod == GETPOST || allowMethod == POSTDELETE || allowMethod == GETPOSTDELETE)))
	{
		_content = dirContent(_serv.getRoot(), req.getQuery());
		_status = "200";
		_method = req.getMethod();
		_contentType = "text/html";
		_contentLength = intToString(_content.length());
	}
	else
	{
		*this = createErrorPage(405, _serv);
	}

}

void Response::respWithLoc(Request& req)
{
	Location loc;
	if (req.getQuery()!= "/")
	{
		if (req.getQuery()[req.getQuery().length() - 1] == '/')
		{
			loc = getTheLocation(req.getQuery());
			if (checkAutoIndex(loc) == false)
			{
				if (getSpecIndex(loc) == "")
					req.setQuery("/");
				else
				{
					std::cout << "Spec index = "<< getSpecIndex(loc) << std::endl;
					req.setQuery(getSpecIndex(loc));
				}
			}
			else
			{
				createAutoIndexResp(req, loc);
				return ;
			}
		}
	}
	loc = getTheLocation(req.getQuery());
	int allowMethod = checkAllowMethod(loc);
	std::cout << "AllowMethod = " << allowMethod << std::endl;
	if (req.getMethod() == "GET" && (allowMethod == GET || allowMethod == GETPOST || allowMethod == GETDELETE || allowMethod == GETPOSTDELETE))
		dealWithGet(req);
	else if (req.getMethod() == "POST" && (allowMethod == POST || allowMethod == GETPOST || allowMethod == POSTDELETE || allowMethod == GETPOSTDELETE))
		dealWithPost(req);
	else if (req.getMethod() == "DELETE" && (allowMethod == DELETE || allowMethod == GETDELETE || allowMethod == POSTDELETE || allowMethod == GETPOSTDELETE))
	{
		std::cout <<"in delete meth" << std::endl;
		dealWithDelete(req);
	}
	else
	{
		std::cout << "Enter here " << std::endl;
		*this = createErrorPage(405, _serv);
	}
}

void Response::respWithOutLoc(Request& req)
{
	if (req.getQuery()!= "/")
	{
		if (req.getQuery()[req.getQuery().length() - 1] == '/')
		{
			std::cout << "enter here7" << std::endl;
			req.setQuery("/");
		}
	}
	if (req.getMethod() == "GET")
	{
		std::cout << "IN GET" << std::endl;
		dealWithGet(req);
	}
	else if (req.getMethod() == "POST")
	{
		std::cout << "IN POST" << std::endl;
		dealWithPost(req);
	}
	else if (req.getMethod() == "DELETE")
	{
		std::cout << "IN DELETE" << std::endl;
		dealWithDelete(req);
	}
	else
	{
		*this = createErrorPage(404, _serv);
	}
}

Response::Response(Request& req, Server serv)
{
	_readFileAccess = OK;
	_serv = serv;
	if (checkIfLocation(req.getQuery()) != -1)
	{
		std::cout << "EEEEEEEEEEEEEEE" << std::endl;
		respWithLoc(req);
	}
	else
	{
		respWithOutLoc(req);
	}
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
