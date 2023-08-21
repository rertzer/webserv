/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileDesc.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 13:48:57 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/21 13:32:47 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileDesc.hpp"
#include "macroDef.hpp"
#include <string.h>

FileDesc::FileDesc(std::string path, struct dirent * sd)
{
	struct stat	statbuf;

	name = sd->d_name;
	path += name;
	if (lstat(path.c_str(), &statbuf) == -1)
	{
		std::cout << strerror(errno) << std::endl;
		//throw (ErrorException(500));
	}
	size = statbuf.st_size;
	type = statbuf.st_mode & S_IFMT;
	last_modified = std::ctime(& statbuf.st_mtim.tv_sec);
}

FileDesc::FileDesc(FileDesc const & rhs)
{
		*this = rhs;
}

FileDesc &	FileDesc::operator=(FileDesc const & rhs)
{

	if (this != &rhs)
	{
		name = rhs.name;
		last_modified = rhs.last_modified;
		type = rhs.type;
		size = rhs.size;
	}
	return *this;
}

FileDesc::~FileDesc()
{}

std::string	FileDesc::getName() const
{
	return name;
}

std::string	FileDesc::getLastModified() const
{
	return last_modified;
}

std::string	FileDesc::getTypeName() const
{
	std::map<mode_t, std::string> typenames;
	typenames[S_IFSOCK] = "socket";
	typenames[S_IFLNK] = "symbolic link";
	typenames[S_IFREG] = "regular file";
	typenames[S_IFBLK] = "block device";
	typenames[S_IFDIR] = "directory";
	typenames[S_IFCHR] = "character device";
	typenames[S_IFIFO] = "FIFO";

	return typenames[type];
}

unsigned int	FileDesc::getSize() const
{
	return size;
}

bool	FileDesc::isDirectory() const
{
	if (type == S_IFDIR)
		return true;
	return false;
}

// private
FileDesc::FileDesc()
{}
