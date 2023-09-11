/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 14:49:31 by pjay              #+#    #+#             */
/*   Updated: 2023/09/11 16:19:38 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "TCPSocket.hpp"

void checkExec(std::string filePath)
{
	if (access(filePath.c_str(), F_OK) == -1)
	{
		std::cout << "throwed here" << std::endl;
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



void Response::feelPart(Request req)
{
	if (req.getQuery() == "/")
	{
		std::string fileStr;
		for (std::vector<std::string>::iterator it = _serv.getDefaultPage().begin(); it != _serv.getDefaultPage().end(); it++)
		{
			std::cout << "root + *it = " << _root + *it << std::endl;
			fileStr = readFile(_root + *it);
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
		//std::cout << "_root + req.getQuery() aaaaa= " << _root + req.getQuery() << std::endl;
		fileStr = readFile(_root + req.getQuery());
		//std::cout << "Content that is not root " << fileStr << std::endl;
		if (fileStr == "404" && _readFileAccess == FILE_NOT_FOUND)
		{
			std::cout << "throwed here 2" << std::endl;
			throw (ErrorException(404));
			//*this = createErrorPage(404, _serv);
		}
		else if (fileStr == "403" && _readFileAccess == ACCESS_DENIED)
			*this = createErrorPage(403, _serv);
		else
		{
			_status = "200 OK";
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

int CheckForRedirection(Location loc)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	while (it != lineLoc.end())
	{
		if (it->getCmd() == "return")
			return (1);
		it++;
	}
	return (0);
}

void Response::dealWithDelete(Request req)
{
	_method = "DELETE";
	checkExec(_root + req.getQuery());
	if (std::remove((_root + req.getQuery()).c_str()) != 0)
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
		_location = redirection.second.substr(0, redirection.second.length() - 2);
		std::cout << "location = " << _location << std::endl;
		req.getSocket()->setKeepAlive(false);
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
	//std::cout << "bedor Allowed methods = " << _allowedMethods << std::endl;
	std::cout << "Cgi status is " << req.getCgiStatus() << std::endl;
	if (req.getCgiStatus() == 4)
	{
		if (respWithCgi(req) == 0)
			return;
	}
	if (checkIfLocation(req.getQuery()) != -1)
	{
		//std::cout << "EEEEEEEEEEEEEEE" << std::endl;
		if (respWithLoc(req) == 0)
			return ;
	}
	else
	{
		if (respWithOutLoc(req) == 0)
			return ;
	}
	//std::cout << "after Allowed methods = " << _allowedMethods << std::endl;
	if (req.getMethod() == "GET" && (_allowedMethods == GET || _allowedMethods == GETPOST || _allowedMethods == GETDELETE || _allowedMethods == GETPOSTDELETE))
		dealWithGet(req);
	else if (req.getMethod() == "POST" && (_allowedMethods == POST || _allowedMethods == GETPOST || _allowedMethods == POSTDELETE || _allowedMethods == GETPOSTDELETE))
		dealWithPost(req);
	else if (req.getMethod() == "DELETE" && (_allowedMethods == DELETE || _allowedMethods == GETDELETE || _allowedMethods == POSTDELETE || _allowedMethods == GETPOSTDELETE))
	{
		//std::cout <<"in delete meth" << std::endl;
		dealWithDelete(req);
	}
	else
	{
		//std::cout << "Enter here " << std::endl;
		*this = createErrorPage(405, _serv);
	}
	//std::cout << "OUT RESPONSE CONSTRUCTOR" << std::endl << std::endl << std::endl;
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
