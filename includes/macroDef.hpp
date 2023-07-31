/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macroDef.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 11:29:20 by pjay              #+#    #+#             */
/*   Updated: 2023/07/31 09:48:08 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <fstream>
#include <exception>
#include <vector>
#include <map>


//CFNG stand for Conf File No Good
#define CONF_FILE_NO_GOOD std::cout << "The conf file doesn't respect subject requirement" << std::endl;
#define CFNG_SMI_COLON std::cout << "A line in the conf File isn't ending with a semicolon" << std::endl;
#define CFNG_BRAC_OPEN std::cout << "A close bracket appear that isn't open anywhere" << std::endl;
#define CFNG_BRAC_CLOSE std::cout << "A bracket is not close in the conf file" << std::endl;

enum parsEnd {BRAC_OPEN, BRAC_CLOSE, SMI_COL, BAD_END};

class Server;

int checkConfFile(char *av);
int fillServ(char *av , std::vector<Server>& serv);
