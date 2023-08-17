/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 14:49:31 by pjay              #+#    #+#             */
/*   Updated: 2023/08/17 09:44:48 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
// GET /repertoire/page.html HTTP/1.1

//  std::string response = "HTTP/1.1 200 OK\r\n"
//                                "Content-Type: text/html\r\n"
//                                "Content-Length: 24\r\n"
//                                "\r\n"
//                                "<html><body>Hello</body></html>";

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
		return(fileStr.str());
	}
	else
	{
		fileOp.close();
		_readFileAccess = FILE_NOT_FOUND;
		return ("404");
	}
}

//We need to add something to find default server
//First : We find the hostname corresponding to the request
//Second : We find the port corresponding to the request
//if he match both we return the server

void Response::feelPart(Request req)
{
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
			_content = fileStr;
			_contentLength = intToString(_content.length()); // mettre en string
		}
	}
	else
	{
		std::string fileStr = readFile(_serv.getRoot() + req.getQuery());
		if (fileStr == "404" && _readFileAccess == FILE_NOT_FOUND)
			CreateErrorPage(404);
		else if (fileStr == "403" && _readFileAccess == ACCESS_DENIED)
			CreateErrorPage(403);
		else
		{
			_status = "200 OK";
			_contentType = getContentKey(req.getQuery().substr(req.getQuery().rfind(".") + 1, req.getQuery().length()));;
			_content = fileStr;
			_contentLength = intToString(_content.length());
		}
	}
}


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
	std::cout << "codeErr = " << codeErr << std::endl;
	_status = Status::getMsg(codeErr);
	_contentType = getContentKey(_serv.getErrorPage(intToString(codeErr)).substr(_serv.getErrorPage(intToString(codeErr)).rfind(".") + 1, _serv.getErrorPage(intToString(codeErr)).length()));
	_content = readFile(_serv.getRoot() + _serv.getErrorPage(intToString(codeErr)));
	_contentLength = intToString(_content.length());
	_connectionClose = "close";
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
		CreateErrorPage(500);
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

std::string Response::getContentKey(std::string value)
{
	std::map<std::string, std::string>::iterator it = _allContentType.find(value);
	if (value == ".py" || value == ".php")
		return "text/html";
	if (it == _allContentType.end())
	{
		std::cout << "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP" << std::endl;
		throw (ErrorException(404));
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

int Response::checkIfLocation(std::string path)
{
	std::vector<Location> loc = _serv.getAllLocation();
	std::vector<Location>::iterator it = loc.begin();
	if (path != "/")
		path = path.substr(0, path.rfind("/"));
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
		path = path.substr(0, path.rfind("/"));
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

int checkAllowMethod(Location loc)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	bool get = false;
	bool post = false;
	bool deleteMethod = false;
	while (it != lineLoc.end())
	{
		std::cout << "cmd = " << it->getCmd() << std::endl;
		if (it->getCmd() == "allow_methods")
		{
			if (it->checkArgs("GET") == 1)
				get = true;
			if (it->checkArgs("POST") == 1)
				post = true;
			if (it->checkArgs("DELETE") == 1)
				deleteMethod = true;
		}
		it++;
	}
	if (get && post && deleteMethod)
		return (GETPOSTDELETE);
	if (get && post)
		return (GETPOST);
	if (get && deleteMethod)
		return (GETDELETE);
	if (post && deleteMethod)
		return (POSTDELETE);
	if (get)
		return (GET);
	if (post)
		return (POST);
	if (deleteMethod)
		return (DELETE);
	return (-1);
}

Response::Response(Request& req, std::vector<Server> serv, int motherPort)
{
	_readFileAccess = OK;
	iniateContentMap();

	_serv = findTheServ(req, serv, motherPort);
	if (checkIfLocation(req.getQuery()) != -1)
	{
		Location loc = getTheLocation(req.getQuery());
		std::cout << "location found" << loc.getLocationPath() <<std::endl;
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
			CreateErrorPage(405);
		}
	}
	else
	{
		if (req.getMethod() == "GET")
		{
			dealWithGet(req);
		}
		else if (req.getMethod() == "POST")
		{
			dealWithPost(req);
		}
		else if (req.getMethod() == "DELETE")
		{
			dealWithDelete(req);
		}
		else
		{

			CreateErrorPage(405);
		}
	}
}

