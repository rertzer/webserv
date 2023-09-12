/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/04 10:56:27 by pjay              #+#    #+#             */
/*   Updated: 2023/09/09 15:21:08 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "macroDef.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "Status.hpp"
#include "ContentMap.hpp"
#include "Cgi.hpp"

class TCPSocket;
enum codeProb{
	OK = 200,
	FILE_NOT_FOUND = 404,
	ACCESS_DENIED = 403,
	METHOD_NOT_ALLOWED = 405,
};

class Response
{
	private:
		Server		_serv;
		std::string _method;
		std::string _status;
		std::string _contentType;
		std::string _contentLength;
		std::string _connectionClose;
		std::string _content;
		std::vector<std::string>	_setCookie;
		std::pair<std::string, std::string> _extensionAllowed;
		int 		_readFileAccess;
		std::string _autoIndex;
		ContentMap	_contentMap;
		std::string _location;
		std::string _root;
		int			_allowedMethods;

		std::pair<std::string, std::string>	extractField(size_t pos);
	public:
		Response(Request& req, Server& serv);
		Response(std::string status, std::string contentType, std::string contentLength, std::string connectionClose, std::string content);
		Response&	operator=(Response const & rhs);
		void 		dealWithGet(Request req);
		void 		dealWithPost(Request req);
		void 		dealWithDelete(Request req);
		std::string	readFile(std::string file);
		std::string	runFile(Request & req);
		std::string	getResponse();
		std::string	getContentKey(std::string value);
		std::string	findLocation(std::string path);
		int 		checkIfLocation(std::string path);
		Location 	getTheLocation(std::string path);
		void 		iniateContentMap();
		void 		feelPart(Request req);
		int 		respWithLoc(Request &req);
		int			initCgi(Request & req, Location & loc);
		int			respWithCgi(Request & req);
		int 		respWithOutLoc(Request& req);
		std::string	getSpecIndex(Location loc);
		void 		createAutoIndexResp(Request& req, Location loc);
		void								setCookie(std::string ck);
		void								Setserv(Server);
		void								setMethod(std::string);
		void								setStatus(std::string);
		void								setContentType(std::string);
		void								setContentLength(std::string);
		void								setConnectionClose(std::string);
		void								setContent(std::string);
		void								setLocation(std::string);
		void								setRoot(std::string);
		void								setAutoIndex(std::string);
		void								setExtensionAllowed(std::pair<std::string, std::string>);
		void								setReadFileAccess(int);
		void								setContentMap(ContentMap);
		void								setAllowedMethods(int);
		//getter
		Server								getServ(void);
		std::string 						getMethod(void);
		std::string 						getStatus(void);
		std::string 						getContentType(void);
		std::string 						getContentLength(void);
		std::string 						getConnectionClose(void);
		std::string 						getContent(void);
		std::string 						getLocation(void);
		std::string 						getRoot(void);
		std::string 						getAutoIndex(void);
		std::vector<std::string>			getSetCookie(void);
		std::pair<std::string, std::string> getExtensionAllowed(void);
		int 								getReadFileAccess(void);
		ContentMap							getContentMap(void);
		int									getAllowedMethods(void);
}
