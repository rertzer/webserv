/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dealWith.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 13:50:13 by pjay              #+#    #+#             */
/*   Updated: 2023/09/12 13:50:56 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "TCPSocket.hpp"
#include "macroDef.hpp"

void dealWithGet(Request req, Response &rep)
{
	rep.setMethod("GET");
	feelPart(req, rep);
}

void dealWithPost(Request req, Response &rep)
{
	rep.setMethod("POST");
	feelPart(req, rep);
}

void dealWithDelete(Request req, Response &rep)
{
	rep.setMethod("DELETE");
	checkExec(rep.getRoot() + req.getQuery());
	if (std::remove((rep.getRoot() + req.getQuery()).c_str()) != 0)
	{
		rep = createErrorPage(404, rep.getServ());
	}
	else
	{
		rep.setStatus("200 OK");
		rep.setContentType("text/html");
		rep.setContent("<html><body>File deleted</body></html>");
		rep.setContentLength(intToString(rep.getContent().length()));
	}
}
