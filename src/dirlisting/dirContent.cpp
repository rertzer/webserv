/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dirContent.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 14:47:33 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/10 16:44:42 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <sstream>
#include "DirListing.hpp"

std::string	dirContent(std::string root, std::string path)
{
	DirListing	drl(root + path);
	
	std::stringstream content;
	content << "<!DOCTYPE html><html><head><title>Index of ";
	content << path;
	content << "</title><meta charset=\"UTF-8\"><link href=\"/css/webserv.css\"";
	content << " rel=\"stylesheet\">";
	content <<	"<link rel=\"icon\" type=\"image/png\" href=\"/favicon-32x32.png\" />";
	content << "<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">";
	content << "<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>";
	content << "<link href=\"https://fonts.googleapis.com/css2?family=Dancing+Script&display=swap\" rel=\"stylesheet\">";
	content << "</head><body><div id=\"container\"><header>";
	content << "<h1>Index of " << path;
	content << "</h1></header><section><table id=\"dircontent\"><tr><th>Name</th><th>Type</th><th>Size (bytes)</th><th>Last Modified</th></tr>";

	std::vector<FileDesc> files = drl.getDirContent();
	for (std::vector<FileDesc>::iterator it = files.begin(); it != files.end(); it++)
	{
		content << "<tr><td><a href=\"" << it->getName() << "\">" <<it->getName() << "</a></td><td>" << it->getTypeName();
		content << "</td><td>" << it->getSize() << "</td><td>" << it->getLastModified();
		content << "</td></tr>";
	}

	content << "</table></section><footer><p>";
	content << "<span class=\"sign\"><b>Webserv</b> by pjay and rertzer</span>";
	content << "<span class=\"devise\">Parce que je le code bien.</span></p>";
	content << "</footer></div></body></html>";
	return content.str();
}
