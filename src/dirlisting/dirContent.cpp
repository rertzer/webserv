/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dirContent.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 14:47:33 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/10 15:08:32 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include "DirListing.hpp"

std::string	dirContent(std::string root, std::string path)
{
	DirListing	drl(root + path);
	
	std::string content = "<DOCTYPE html><html><head><title>Index of ";
	content += path;
	content += "</title></head><body><h1>Index of ";
	content += path;
	content += "</h1><table>";

	std::vector<FileDesc> files = drl.getDirContent();
	for (std::vector<FileDesc>::iterator it = files.begin(); it != files.end(); it++)
	{
		content += "<tr><th>" + it->getName() + "</th><th>" + it->getTypeName();
		content += "</th><th>" + it->getSize() + "</th><th>" + it->getLastModified();
		content += "</th></tr>";
	}

	content += "</table></body></html>";
	return content;
}
