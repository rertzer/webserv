/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 14:49:31 by pjay              #+#    #+#             */
/*   Updated: 2023/08/03 14:55:14 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "macroDef.hpp"
#include "Request.hpp"
#include "Server.hpp"
// GET /repertoire/page.html HTTP/1.1

//  std::string response = "HTTP/1.1 200 OK\r\n"
//                                "Content-Type: text/html\r\n"
//                                "Content-Length: 24\r\n"
//                                "\r\n"
//                                "<html><body>Hello</body></html>";


std::string readFile(std::string file)
{
	std::ifstream fileOp;
	fileOp.open(file);
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

std::string sendResponse(Request& req, Server& serv)
{
	std::string fileStr = readFile(serv.getRoot() + req.getQuery());
	if (req.getQuery() == '/')
	{
		for (std::vector<std::string>::iterator it = serv.getDefaultPage().begin(); it != serv.getDefaultPage().end(); it++)
		{
			if ((fileStr = readFile(serv.getRoot() + *it)) != "404")
				break;
		}
	}
	else if (fileStr == "404")
	{
		fileStr = readFile(serv.getRoot() + serv.getErrorPage("404"));
		return ("HTTP/1.1 404 Not Found\r\n\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length: " + std::to_string(fileStr.length()) + "\r\n"
					"\r\n"
					+ fileStr);
	}
	std::string response = "HTTP/1.1 200 OK\r\n"
								"Content-Type: text/html\r\n"
								"Content-Length: " + std::to_string(fileStr.length()) + "\r\n"
								"\r\n"
								+ fileStr;
	return (response);
}
