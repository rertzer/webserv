/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/04 10:56:27 by pjay              #+#    #+#             */
/*   Updated: 2023/08/10 14:14:31 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "macroDef.hpp"
#include "Request.hpp"
#include "Server.hpp"

class Response
{
	private:
		Server _serv;
		std::string _method;
		std::string _status;
		std::string _contentType;
		std::string _contentLength;
		std::string _content;
	public:
		Response(Request& req, std::vector<Server> serv, int motherPort);
		Response(std::vector<Server> serv, int codeErr);
		void dealWithGet(Request req);
		std::string getResponse();
};
