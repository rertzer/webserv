/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 14:49:31 by pjay              #+#    #+#             */
/*   Updated: 2023/08/17 11:31:12 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
// GET /repertoire/page.html HTTP/1.1

//  std::string response = "HTTP/1.1 200 OK\r\n"
//                                "Content-Type: text/html\r\n"
//                                "Content-Length: 24\r\n"
//                                "\r\n"
//                                "<html><body>Hello</body></html>";


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

	std::cout << "GLOUGLOUGLOUGLOUGLOU " << myCgi.getPath() << std::endl; 
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

//We need to add something to find default server
//First : We find the hostname corresponding to the request
//Second : We find the port corresponding to the request
//if he match both we return the server
Server findTheServ(Request& req, std::vector<Server>& serv, int motherPort)
{
	std::vector<Server>::iterator it = serv.begin();
	while (it != serv.end())
	{
		if (req.getField("Host") == it->getServName())
		{
			if (it->getListenPort().size() > 1)
			{
				for (std::vector<int>::iterator it2 = it->getListenPort().begin(); it2 != it->getListenPort().end(); it2++)
				{
					if (motherPort == *it2)
						return (*it);
				}
			}
			else
			{
				if (motherPort == *it->getListenPort().begin())
						return (*it);
			}

		}
		it++;
	}
	return (*(serv.begin()));
}

void Response::CreateErrorPage(int codeErr)
{
	_status = Status::getMsg(codeErr);
	_contentType = getContentKey(_serv.getErrorPage(intToString(codeErr)).substr(_serv.getErrorPage(intToString(codeErr)).rfind(".") + 1, _serv.getErrorPage(intToString(codeErr)).length()));
	_content = readFile(_serv.getRoot() + _serv.getErrorPage(intToString(codeErr)));
	_contentLength = intToString(_content.length());
}

void Response::dealWithGet(Request req)
{
	_method = "GET";

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
				_contentType = getContentKey(it->substr(it->rfind(".") + 1, it->length()));
				break;
			}
		}
		if (fileStr == "404" && _readFileAccess == FILE_NOT_FOUND)
			CreateErrorPage(404);
		else if (fileStr == "403" && _readFileAccess == ACCESS_DENIED)
			CreateErrorPage(403);
		else
		{
			_status = "200 OK";
			//std::cout << "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ" << req.getQuery().substr();
			_content = fileStr;
			_contentLength = intToString(_content.length()); // mettre en string
		}
	}
	else
	{
		std::string	fileStr;
		if (req.getQuery().find(".php") != std::string::npos)
		{
			fileStr= runFile("GET", req); 
		}
		else
		{
			std::cout << "_serv.getRoot() + req.getQuery() = " << _serv.getRoot() + req.getQuery() << std::endl;
			fileStr = readFile(_serv.getRoot() + req.getQuery());
			//std::cout << "Content that is not root " << fileStr << std::endl;
		}
		if (fileStr == "404" && _readFileAccess == FILE_NOT_FOUND)
		CreateErrorPage(404);
		else if (fileStr == "403" && _readFileAccess == ACCESS_DENIED)
			CreateErrorPage(403);
		else
		{
			_status = "200 OK";
			if (req.getQuery().find(".php") != std::string::npos)
				_contentType = "text/html";
			else
				_contentType = getContentKey(req.getQuery().substr(req.getQuery().rfind(".") + 1, req.getQuery().length()));;
			_content = fileStr;
			_contentLength = intToString(_content.length());
		//	std::cout << "Content that is not root " << fileStr << std::endl;
		}
	}
}

void Response::dealWithPost(Request req)
{
	_method = "POST";
	if (req.getQuery() == "/")
	{
		std::string fileStr;
		for (std::vector<std::string>::iterator it = _serv.getDefaultPage().begin(); it != _serv.getDefaultPage().end(); it++)
		{
			fileStr = readFile(_serv.getRoot() + *it);
			if (fileStr == "403" && _readFileAccess == ACCESS_DENIED)
				break;
			if (fileStr != "404" && _readFileAccess != FILE_NOT_FOUND)
			{
				_contentType = getContentKey(it->substr(it->rfind(".") + 1, it->length()));
				break;
			}
		}
		//std::cout << "file found = " << fileStr << std::endl;
		if (fileStr == "404" && _readFileAccess == FILE_NOT_FOUND)
			CreateErrorPage(404);
		else if (fileStr == "403" && _readFileAccess == ACCESS_DENIED)
			CreateErrorPage(403);
		else
		{
			std::cout << "Accept field = " << req.getField("Accept") << std::endl;
			_status = "200 OK";
			_contentType = getContentKey(req.getQuery().substr(req.getQuery().rfind(".") + 1, req.getQuery().length()));;
			_content = fileStr;
			_contentLength = intToString(_content.length()); // mettre en string
		}
	}
	else
	{
		//std::cout << "_serv.getRoot() + req.getQuery() = " << _serv.getRoot() + req.getQuery() << std::endl;
		std::string	fileStr;
		if (req.getQuery().find(".php") != std::string::npos)
		{
			fileStr= runFile("POST", req); 
		}
		else
		{
			fileStr = readFile(_serv.getRoot() + req.getQuery());
			std::cout << "Content that is not root " << fileStr << std::endl;
		}
		if (fileStr == "404" && _readFileAccess == FILE_NOT_FOUND)
			CreateErrorPage(404);
		else if (fileStr == "403" && _readFileAccess == ACCESS_DENIED)
			CreateErrorPage(403);
		else
		{
			_status = "200 OK";
			_contentType = getContentKey(req.getQuery().substr(req.getQuery().rfind(".") + 1, req.getQuery().length()));
			_content = fileStr;
			_contentLength = intToString(_content.length());
		//	std::cout << "Content that is not root " << fileStr << std::endl;
		}
	}
}

std::string Response::getResponse()
{
	std::string response = "HTTP/1.1 " + _status + " \r\n"
							"Content-Type: " + _contentType + "\r\n"
							"Content-Length: " + _contentLength + "\r\n"
							"\r\n"
							+ _content;
	return (response);
}

std::string Response::getContentKey(std::string value)
{
	if (value == "php")
		return ("text/html");
	if (value == "py")
		return ("text/html");
	
	std::map<std::string, std::string>::iterator it = _allContentType.find(value);
	if (it == _allContentType.end())
	{
		std::cout << "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP" << std::endl;
		throw (ErrorException(418));
	}
	else
	{
		std::cout << "Content key = found " << it->second + "/" + value << std::endl;
		return (it->second + "/" + value);
	}
}

void Response::iniateContentMap()
{
	_allContentType["EDI-X12"] = "application";
	_allContentType["EDIFACT"] = "application";
	_allContentType["javascript"] = "application";
	_allContentType["octet-stream"] = "application";
	_allContentType["ogg"] = "application";
	_allContentType["pdf"] = "application";
	_allContentType["xhtml+xml"] = "application";
	_allContentType["x-shockwave-flash"] = "application";
	_allContentType["json"] = "application";
	_allContentType["ld+json"] = "application";
	_allContentType["xml"] = "application";
	_allContentType["zip"] = "application";
	_allContentType["x-www-form-urlencoded"] = "application";
	_allContentType["mpeg"] = "audio";
	_allContentType["x-ms-wma"] = "audio";
	_allContentType["vnd.rn-realaudio"] = "audio";
	_allContentType["x-wav"] = "audio";
	_allContentType["gif"] = "image";
	_allContentType["jpeg"] = "image";
	_allContentType["png"] = "image";
	_allContentType["tiff"] = "image";
	_allContentType["vnd.microsoft.icon"] = "image";
	_allContentType["x-icon"] = "image";
	_allContentType["vnd.djvu"] = "image";
	_allContentType["svg+xml"] = "image";
	_allContentType["mixed"] = "multipart";
	_allContentType["alternative"] = "multipart";
	_allContentType["related"] = "multipart";
	_allContentType["form-data"] = "multipart";
	_allContentType["css"] = "text";
	_allContentType["csv"] = "text";
	_allContentType["html"] = "text";
	_allContentType["javascript"] = "text";
	_allContentType["plain"] = "text";
	_allContentType["xml"] = "text";
	_allContentType["mpeg"] = "video";
	_allContentType["mp4"] = "video";
	_allContentType["quicktime"] = "video";
	_allContentType["x-ms-wmv"] = "video";
	_allContentType["x-msvideo"] = "video";
	_allContentType["x-flv"] = "video";
	_allContentType["webm"] = "video";
	_allContentType["vnd.oasis.opendocument.text"] = "application";
	_allContentType["vnd.oasis.opendocument.spreadsheet"] = "application";
	_allContentType["vnd.oasis.opendocument.presentation"] = "application";
	_allContentType["vnd.oasis.opendocument.graphics"] = "application";
	_allContentType["vnd.ms-excel"] = "application";
	_allContentType["vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = "application";
	_allContentType["vnd.ms-powerpoint"] = "application";
	_allContentType["vnd.openxmlformats-officedocument.presentationml.presentation"] = "application";
	_allContentType["msword"] = "application";
	_allContentType["vnd.openxmlformats-officedocument.wordprocessingml.document"] = "application";
	_allContentType["vnd.mozilla.xul+xml"] = "application";
}

Response::Response(Request& req, std::vector<Server> serv, int motherPort)
{
	_readFileAccess = OK;
	iniateContentMap();
	_serv = findTheServ(req, serv, motherPort);
	if (req.getMethod() == "GET")
	{
		dealWithGet(req);
	}
	if (req.getMethod() == "POST")
	{
		dealWithPost(req);
	}
}

