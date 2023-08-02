/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfFile.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 11:22:52 by pjay              #+#    #+#             */
/*   Updated: 2023/07/27 11:44:48 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "macroDef.hpp"

class ConfFile
{
	private :
		ConfFile();
	public :
		ConfFile(const ConfFile& rhs);
		ConfFile operator=(const ConfFile& rhs);
		~ConfFile();
		ConfFile(char *av);
		int setServName(char *av);
		int setServSide(char *av);
		std::string getServName();
		struct sockaddr_in getServSide();
};
