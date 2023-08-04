/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 11:13:19 by pjay              #+#    #+#             */
/*   Updated: 2023/08/04 12:18:36 by pjay             ###   ########.fr       */
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
		Server();
		Server(std::vector<std::string> servStrings);
		Server(const Server& rhs);
		~Server();
		Server& operator=(const Server& rhs);
		std::vector<std::string>& getDefaultPage();
		std::string& getServName();
		std::string& getRoot();
		std::string getErrorPage(std::string errorNb);
		std::map<std::string, std::string>& getAllErrorPage();
		std::vector<Location>& getAllLocation();
		std::vector<int>& getListenPort();
};
