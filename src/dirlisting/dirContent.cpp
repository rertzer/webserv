/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dirContent.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 14:47:33 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/12 11:12:02 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <sstream>
#include "DirListing.hpp"

std::string	dirContent(std::string root, std::string path)
{
	DirListing	drl(root + path);
	
	std::stringstream content;
	content << "<DOCTYPE html><html><head><title>Index of ";
	content << path;
	content << "</title></head><body><h1>Index of ";
	content << path;
	content << "</h1><table><tr><th>Name</th><th>Type</th><th>Size (bytes)</th><th>Last Modified</th></tr>";

	std::vector<FileDesc> files = drl.getDirContent();
	for (std::vector<FileDesc>::iterator it = files.begin(); it != files.end(); it++)
	{
		content << "<tr><th><a href=\"" << it->getName() << "\">" <<it->getName() << "</a></th><th>" << it->getTypeName();
		content << "</th><th>" << it->getSize() << "</th><th>" << it->getLastModified();
		content << "</th></tr>";
	}

	content << "</table></body></html>";
	return content.str();
}
