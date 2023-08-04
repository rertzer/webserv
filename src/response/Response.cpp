/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 14:49:31 by pjay              #+#    #+#             */
/*   Updated: 2023/08/04 14:42:40 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
// GET /repertoire/page.html HTTP/1.1

//  std::string response = "HTTP/1.1 200 OK\r\n"
//                                "Content-Type: text/html\r\n"
//                                "Content-Length: 24\r\n"
//                                "\r\n"
//                                "<html><body>Hello</body></html>";


std::string readFile(std::string file)
{
	std::ifstream fileOp;
	fileOp.open(file.c_str());
	std::cout << "File to open = " << file << std::endl;
	if (fileOp.is_open())
	{
		std::string stocked;
		std::string fileStr;
		while (getline(fileOp, stocked))
		{
			fileStr += stocked;
		}
		fileOp.close();
		return(fileStr);
	}
	else
	{
		fileOp.close();
		return ("404");
	}
}

//We need to add something to find default server
//First : We find the hostname corresponding to the request
//Second : We find the port corresponding to the request
//if he match both we return the server
Server findTheServ(Request& req, std::vector<Server>& serv)
{
	std::vector<Server>::iterator it = serv.begin();
	std::cout << "Number of serv " << serv.size() << std::endl;
	std::cout << "Req port = " << req.getPort() << std::endl;
	while (it != serv.end())
	{
		std::cout << "Rotation" << std::endl;
		if (req.getField("Host").substr(0, req.getField("Host").find(":")) == it->getServName())
		{
			for (std::vector<int>::iterator it2 = it->getListenPort().begin(); it2 != it->getListenPort().end(); it2++)
			{
				if (atoi(req.getField("Host").substr(req.getField("Host").find(":") + 1, req.getField("Host").length()).c_str()) == *it2)
					return (*it);
			}
		}
		it++;
	}
	return (*it);
}

void Response::dealWithGet(Request req)
{
	_method = "GET";
	if (req.getQuery() == "/")
	{
		std::string fileStr = readFile(_serv.getRoot() + req.getQuery());
		for (std::vector<std::string>::iterator it = _serv.getDefaultPage().begin(); it != _serv.getDefaultPage().end(); it++)
		{
			fileStr = readFile(_serv.getRoot() + *it);
			if (fileStr != "404")
				break;
		}
		std::cout << "file found = " << fileStr << std::endl;
		if (fileStr == "404")
		{
			_status = "404 Not Found";
			_contentType = req.getField("Accept").substr(0, req.getField("Accept").find(","));
			_contentLength = fileStr.length();
			_content = readFile(_serv.getRoot() + _serv.getErrorPage("404"));
		}
		else
		{
			_status = "200 OK";
			_contentType = req.getField("Accept").substr(0, req.getField("Accept").find(","));
			_contentLength = fileStr.length();
			_content = fileStr;
		}
	}
	else
	{
		std::cout << "_serv.getRoot() + req.getQuery() = " << _serv.getRoot() + req.getQuery() << std::endl;
		std::string fileStr = readFile(_serv.getRoot() + req.getQuery());
		std::cout << "Content that is not root " << fileStr << std::endl;
		if (fileStr == "404")
		{
			_status = "404 Not Found";
			_contentType = req.getField("Accept").substr(0, req.getField("Accept").find(","));
			_content = readFile(_serv.getRoot() + _serv.getErrorPage("404"));
			_contentLength = fileStr.length();
		}
		else
		{
			_status = "200 OK";
			_contentType = req.getField("Accept").substr(0, req.getField("Accept").find(","));
			_content = fileStr;
			_contentLength = fileStr.length();
			std::cout << "Content that is not root " << fileStr << std::endl;
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

Response::Response(Request& req, std::vector<Server> serv)
{
	_serv = findTheServ(req, serv);
	if (req.getMethod() == "GET")
	{
		dealWithGet(req);
	}
}

