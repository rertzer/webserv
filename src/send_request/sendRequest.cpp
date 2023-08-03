/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sendRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 14:49:31 by pjay              #+#    #+#             */
/*   Updated: 2023/08/03 13:51:20 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "macroDef.hpp"
#include "Request.hpp"
// GET /repertoire/page.html HTTP/1.1

//  std::string response = "HTTP/1.1 200 OK\r\n"
//                                "Content-Type: text/html\r\n"
//                                "Content-Length: 24\r\n"
//                                "\r\n"
//                                "<html><body>Hello</body></html>";


std::string readFile(std::string &file)
{
	std::ifstream fileOp(file);
	std::string stocked;
	std::string fileStr;
	if (fileOp.open())
	{
		while (getline(fileOp, stocked))
		{
			fileStr += stocked;
		}

	}
	else
	{
		return ("unable to open this file");
	}
}


std::string sendResponse(Request req)
{
	if (req.getQuery)
}
