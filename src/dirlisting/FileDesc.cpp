/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileDesc.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 13:48:57 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/10 14:35:07 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileDesc.hpp"

FileDesc::FileDesc(std::string path, struct dirent sd)
{
	struct stat	statbuf;

	name = sd.d_name;
	path += name;

	if (lstat(path.c_str(), &statbuf) == -1)
		throw (ErrorException(500));
	size = statbuf.st_size;
	type = statbuf.st_mode & S_IFMT;
	last_modified = ctime(statbuf.st_mtimi.tv_sec);
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
	std::map<struct mode_t, std::string> typenames;
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
	if (type == S_ISDIR)
		return true;
	return false;
}

// private
FileDesc::Filedesc()
{}

FileDesc::FileDesc(FileDesc const & rhs)
{
	if (this == &rhs)
		*this = rhs;
}

FileDesc &	FileDesc::operator=(FileDesc const & rhs)
{
	static_cast<void>(rhs);
}
