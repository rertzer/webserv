/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 11:13:19 by pjay              #+#    #+#             */
/*   Updated: 2023/07/31 12:30:19 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "macroDef.hpp"

class Server
{
	private:
		int _servSocket;
		std::string _servName;
		struct sockaddr_in _servSide;
		std::vector<std::string> _defaultPage;
		std::string _root;
		std::map<std::string, std::string> _errorPage;
		std::map<std::string, std::string> _location; // Reflechir a en faire une classe
	public :
		Server(std::vector<std::string> servStrings);
		~Server();
		std::string& getServName();
		int& getServSocket();
		struct sockaddr_in& getServSide();
		std::string& getRoot();
		std::string getErrorPage(std::string errorNb);
		std::map<std::string, std::string>& getAllErrorPage();
		std::map<std::string, std::string>& getAllLocation();

		// int setServName(std::string name);
		// int setServSocket(int socket);
		// int setServSide(struct sockaddr_in side);
		// int setRoot(std::string root);
		// int setErrorPage(std::map<std::string, std::string> errorPage);
		//std::string getLocation(std::string location);
};
