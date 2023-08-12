/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   maintest.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 10:23:32 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/12 10:59:07 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include "ErrorException.hpp"

std::string	dirContent(std::string root, std::string path);

int	main()
{
	try
	{
		std::string root = "/mnt/nfs/homes/rertzer/projets/webserv/webserv_git/";
		std::string path = "src/dirlisting/";
		std::cout << dirContent(root, path) << std::endl;
		return 0;
	}
	catch (ErrorException const &e)
	{
		std::cout << e.what() << e.getCode() << std::endl;
	}
}
