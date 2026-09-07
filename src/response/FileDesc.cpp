#include <time.h>
#include <cstring>
#include <iostream>
#include <map>
#include <string>

#include "webserv/FileDesc.hpp"

FileDesc::FileDesc(std::string path, struct dirent* sd) {
  struct stat statbuf;

  name = sd->d_name;
  path += name;
  if (lstat(path.c_str(), &statbuf) == -1) {
    std::cout << strerror(errno) << std::endl;
    // throw (ErrorException(500));
  }
  size = statbuf.st_size;
  type = statbuf.st_mode & S_IFMT;
  setTime(statbuf);
}

void FileDesc::setTime(struct stat stat_buffer) {
  char timebuf[32];
  ::tm tm_time;

  localtime_r(&stat_buffer.st_mtim.tv_sec, &tm_time);
  ::strftime(timebuf, sizeof(timebuf), "%a %b %d %H:%M:%S %Y", &tm_time);
  last_modified = timebuf;
}

FileDesc::FileDesc(FileDesc const& rhs) {
  *this = rhs;
}

FileDesc& FileDesc::operator=(FileDesc const& rhs) {
  if (this != &rhs) {
    name = rhs.name;
    last_modified = rhs.last_modified;
    type = rhs.type;
    size = rhs.size;
  }
  return *this;
}

FileDesc::~FileDesc() {}

std::string FileDesc::getName() const {
  return name;
}

std::string FileDesc::getLastModified() const {
  return last_modified;
}

std::string FileDesc::getTypeName() const {
  std::map<mode_t, std::string> typenames{{S_IFSOCK, "socket"},      {S_IFLNK, "symbolic link"},
                                          {S_IFREG, "regular file"}, {S_IFBLK, "block device"},
                                          {S_IFDIR, "directory"},    {S_IFCHR, "character device"},
                                          {S_IFIFO, "FIFO"}};
  return typenames[type];
}

unsigned int FileDesc::getSize() const {
  return size;
}

bool FileDesc::isDirectory() const {
  if (type == S_IFDIR)
    return true;
  return false;
}

// private
FileDesc::FileDesc() {}
