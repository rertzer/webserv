
#include <string>

#include "DirListing.hpp"
#include "macroDef.hpp"
#include "utils.hpp"

std::string dirContent(std::string root, std::string path) {
	DirListing drl(root + path);

	auto index = getFileContent("data/index_header.html");

	replaceAll(index, "PATH", path);

	auto				  content = getFileContent("data/index_content.html");
	std::vector<FileDesc> files = drl.getDirContent();
	for (std::vector<FileDesc>::iterator it = files.begin(); it != files.end(); it++) {
		auto new_content = content;
		replaceAll(new_content, "NAME", it->getName());
		replaceAll(new_content, "TYPE", it->getTypeName());
		replaceAll(new_content, "SIZE", std::to_string(it->getSize()));
		replaceAll(new_content, "LASTMODIFIED", it->getLastModified());
		index.append(new_content);
	}
	auto footer = getFileContent("data/index_footer.html");
	index.append(footer);
	return index;
}
