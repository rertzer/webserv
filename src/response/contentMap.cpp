#include "ContentMap.hpp"

std::string ContentMap::getContentValue(std::string value) {
	std::map<std::string, std::string>::iterator it = _allContentType.find(value);
	// if (value == "py" || value == "php")
	// {
	// 	return "text/html";
	// }
	if (it == _allContentType.end()) {
		return "text/plain";
	} else {
		return (it->second + "/" + value);
	}
}

ContentMap::ContentMap() {
	_allContentType = {
		{"php", "application"},
		{"py", "application"},
		{"EDI-X12", "application"},
		{"EDIFACT", "application"},
		{"javascript", "application"},
		{"octet-stream", "application"},
		{"ogg", "application"},
		{"pdf", "application"},
		{"xhtml+xml", "application"},
		{"x-shockwave-flash", "application"},
		{"json", "application"},
		{"ld+json", "application"},
		{"xml", "application"},
		{"zip", "application"},
		{"x-www-form-urlencoded", "application"},
		{"mpeg", "audio"},
		{"x-ms-wma", "audio"},
		{"vnd.rn-realaudio", "audio"},
		{"x-wav", "audio"},
		{"gif", "image"},
		{"jpeg", "image"},
		{"jpg", "image"},
		{"png", "image"},
		{"tiff", "image"},
		{"vnd.microsoft.icon", "image"},
		{"x-icon", "image"},
		{"vnd.djvu", "image"},
		{"svg+xml", "image"},
		{"mixed", "multipart"},
		{"alternative", "multipart"},
		{"related", "multipart"},
		{"form-data", "multipart"},
		{"css", "text"},
		{"csv", "text"},
		{"html", "text"},
		{"javascript", "text"},
		{"plain", "text"},
		{"xml", "text"},
		{"mpeg", "video"},
		{"mp4", "video"},
		{"quicktime", "video"},
		{"x-ms-wmv", "video"},
		{"x-msvideo", "video"},
		{"x-flv", "video"},
		{"webm", "video"},
		{"vnd.oasis.opendocument.text", "application"},
		{"vnd.oasis.opendocument.spreadsheet", "application"},
		{"vnd.oasis.opendocument.presentation", "application"},
		{"vnd.oasis.opendocument.graphics", "application"},
		{"vnd.ms-excel", "application"},
		{"vnd.openxmlformats-officedocument.spreadsheetml.sheet", "application"},
		{"vnd.ms-powerpoint", "application"},
		{"vnd.openxmlformats-officedocument.presentationml.presentation", "application"},
		{"msword", "application"},
		{"vnd.openxmlformats-officedocument.wordprocessingml.document", "application"},
		{"vnd.mozilla.xul+xml", "application"},
		{"webp", "image"},
	};
}
