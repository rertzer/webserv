/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 11:13:19 by pjay              #+#    #+#             */
/*   Updated: 2023/08/03 11:06:28 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "macroDef.hpp"
#include "Location.hpp"

class Server
{
	private:
		std::string _servName;
		std::vector<int> _nPort;
		std::vector<std::string> _defaultPage;
		std::string _root;
		std::map<std::string, std::string> _errorPage;
		std::vector<Location> _location;
	public :
		Server(std::vector<std::string> servStrings);
		~Server();
		std::string& getServName();
		//int& getServSocket();
		//struct sockaddr_in& getServSide();
		std::string& getRoot();
		std::string getErrorPage(std::string errorNb);
		std::map<std::string, std::string>& getAllErrorPage();
		std::vector<Location>& getAllLocation();
		std::vector<int>& getListenPort();
		// int setServName(std::string name);
		// int setServSocket(int socket);
		// int setServSide(struct sockaddr_in side);
		// int setRoot(std::string root);
		// int setErrorPage(std::map<std::string, std::string> errorPage);
		//std::string getLocation(std::string location);
};
