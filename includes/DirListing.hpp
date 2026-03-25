#ifndef DIRLISTING_HPP
#define DIRLISTING_HPP

#include <dirent.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include "FileDesc.hpp"

class DirListing {
   public:
	DirListing(std::string path);
	DirListing(DirListing const& rhs);
	~DirListing();
	DirListing& operator=(DirListing const& rhs);

	std::vector<FileDesc> getDirContent() const;

   private:
	DirListing();

	void setDirContent(DIR* dd);

	std::string			  path;
	std::vector<FileDesc> dir_content;
};

#endif
