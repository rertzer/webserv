/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   contentMap.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 11:37:17 by pjay              #+#    #+#             */
/*   Updated: 2023/08/17 11:40:09 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ContentMap.hpp"

std::string ContentMap::getContentValue(std::string value)
{
	std::map<std::string, std::string>::iterator it = _allContentType.find(value);
	if (value == ".py" || value == ".php")
		return "text/html";
	if (it == _allContentType.end())
	{
		std::cout << "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP" << std::endl;
		throw (ErrorException(404));
	}
	else
	{
		std::cout << "Content key = found " << it->second + "/" + value << std::endl;
		return (it->second + "/" + value);
	}
}

ContentMap::ContentMap()
{
	_allContentType["EDI-X12"] = "application";
	_allContentType["EDIFACT"] = "application";
	_allContentType["javascript"] = "application";
	_allContentType["octet-stream"] = "application";
	_allContentType["ogg"] = "application";
	_allContentType["pdf"] = "application";
	_allContentType["xhtml+xml"] = "application";
	_allContentType["x-shockwave-flash"] = "application";
	_allContentType["json"] = "application";
	_allContentType["ld+json"] = "application";
	_allContentType["xml"] = "application";
	_allContentType["zip"] = "application";
	_allContentType["x-www-form-urlencoded"] = "application";
	_allContentType["mpeg"] = "audio";
	_allContentType["x-ms-wma"] = "audio";
	_allContentType["vnd.rn-realaudio"] = "audio";
	_allContentType["x-wav"] = "audio";
	_allContentType["gif"] = "image";
	_allContentType["jpeg"] = "image";
	_allContentType["png"] = "image";
	_allContentType["tiff"] = "image";
	_allContentType["vnd.microsoft.icon"] = "image";
	_allContentType["x-icon"] = "image";
	_allContentType["vnd.djvu"] = "image";
	_allContentType["svg+xml"] = "image";
	_allContentType["mixed"] = "multipart";
	_allContentType["alternative"] = "multipart";
	_allContentType["related"] = "multipart";
	_allContentType["form-data"] = "multipart";
	_allContentType["css"] = "text";
	_allContentType["csv"] = "text";
	_allContentType["html"] = "text";
	_allContentType["javascript"] = "text";
	_allContentType["plain"] = "text";
	_allContentType["xml"] = "text";
	_allContentType["mpeg"] = "video";
	_allContentType["mp4"] = "video";
	_allContentType["quicktime"] = "video";
	_allContentType["x-ms-wmv"] = "video";
	_allContentType["x-msvideo"] = "video";
	_allContentType["x-flv"] = "video";
	_allContentType["webm"] = "video";
	_allContentType["vnd.oasis.opendocument.text"] = "application";
	_allContentType["vnd.oasis.opendocument.spreadsheet"] = "application";
	_allContentType["vnd.oasis.opendocument.presentation"] = "application";
	_allContentType["vnd.oasis.opendocument.graphics"] = "application";
	_allContentType["vnd.ms-excel"] = "application";
	_allContentType["vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = "application";
	_allContentType["vnd.ms-powerpoint"] = "application";
	_allContentType["vnd.openxmlformats-officedocument.presentationml.presentation"] = "application";
	_allContentType["msword"] = "application";
	_allContentType["vnd.openxmlformats-officedocument.wordprocessingml.document"] = "application";
	_allContentType["vnd.mozilla.xul+xml"] = "application";
}
