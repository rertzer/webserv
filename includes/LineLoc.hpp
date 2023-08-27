/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LineLoc.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 11:02:41 by pjay              #+#    #+#             */
/*   Updated: 2023/08/24 09:49:58 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once
#include "Server.hpp"

class LineLoc
{
	private :
		std::string _cmd;
		std::vector <std::string> _args;
	public :
		std::string& getCmd();
		std::vector <std::string>& getArgs();
		LineLoc(std::string line);
		int checkArgs(std::string toFind);
};
