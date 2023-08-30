#include "DirListing.hpp"

DirListing::DirListing(std::string p):path(p)
{
	DIR	*dd;
	dd = opendir(path.c_str());
	if (dd == NULL)
	{
		throw (ErrorException(500));
	}
	setDirContent(dd);
	closedir(dd);
}

DirListing::DirListing(DirListing const & rhs)
{
	*this = rhs;
}

DirListing::~DirListing()
{}


DirListing &	DirListing::operator=(DirListing const & rhs)
{
	if (this != &rhs)
	{
		path = rhs.path;
		dir_content = rhs.dir_content;
	}
	return *this;
}
std::vector<FileDesc>	DirListing::getDirContent() const
{
	return dir_content;
}

//private
DirListing::DirListing()
{}



void	DirListing::setDirContent(DIR * dd)
{
	struct dirent * entry;

	errno = 0;
	entry = readdir(dd);
	while (entry)
	{
		dir_content.push_back(FileDesc(path, entry));
		entry = readdir(dd);
	}
	if (errno)
		throw (ErrorException(500));
}
