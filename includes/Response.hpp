/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/04 10:56:27 by pjay              #+#    #+#             */
/*   Updated: 2023/08/14 16:51:24 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "macroDef.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "Status.hpp"

enum codeProb{
	OK = 200,
	FILE_NOT_FOUND = 404,
	ACCESS_DENIED = 403,
	METHOD_NOT_ALLOWED = 405,
};

class Response
{
	private:
		Server _serv;
		std::string _method;
		std::string _status;
		std::string _contentType;
		std::string _contentLength;
		int _readFileAccess;
		std::string _content;
		std::map<std::string, std::string> _allContentType;
	public:
		Response(Request& req, std::vector<Server> serv, int motherPort);
		Response(std::vector<Server> serv, int codeErr);
		void dealWithGet(Request req);
		void dealWithPost(Request req);
		std::string readFile(std::string file);
		std::string getResponse();
		void CreateErrorPage(int codeErr);
		std::string getContentKey(std::string value);
		void iniateContentMap();
};
