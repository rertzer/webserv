#include "DirListing.hpp"

DirListing::DirListing(std::string path)
{
	DIR	*dd;

	dd = opendir(path);
	if (dd == NULL)
		throw (ErrorException(500));
	setContent(dd);
	closedir(dd);
}

std::vector<FileDesc>	DirListing::getDirContent() const
{
	return dir_content;
}

//private
DirListing::Dirlisting()
{}

DirListing::DirListing(DirListing const & rhs)
{
	if (this != &rhs)
		*this = rhs;
}

DirListing &	DirListing::operator=(DirListing const & rhs)
{
	static_cast<void>(rhs);
}

void	DirListing::setDirContent(DIR * dd)
{
	struct dirent * entry;
	
	errno = 0;
	entry = readdir(dd);
	while (entry)
	{
		dir_content.push_back(FileDesc(entry));
		entry = readdir(dd);
	}
	if (errno)
		throw (ErrorException(500));
}
