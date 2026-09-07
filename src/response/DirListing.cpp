#include <string>
#include <vector>

#include "webserv/DirListing.hpp"
#include "webserv/ErrorException.hpp"
#include "webserv/HttpStatus.hpp"

DirListing::DirListing(std::string p) : path(p) {
  DIR* dd;
  dd = opendir(path.c_str());
  if (dd == nullptr) {
    HttpStatus error = HttpStatus::INTERNAL_SERVER_ERROR;
    if (errno == ENOTDIR || errno == ENOENT)
      error = HttpStatus::NOT_FOUND;
    else if (errno == EACCES)
      error = HttpStatus::FORBIDDEN;
    throw(ErrorException(error));
  }
  setDirContent(dd);
  closedir(dd);
}

DirListing::DirListing(DirListing const& rhs) {
  *this = rhs;
}

DirListing::~DirListing() {}

DirListing& DirListing::operator=(DirListing const& rhs) {
  if (this != &rhs) {
    path = rhs.path;
    dir_content = rhs.dir_content;
  }
  return *this;
}
std::vector<FileDesc> DirListing::getDirContent() const {
  return dir_content;
}

// private
DirListing::DirListing() {}

void DirListing::setDirContent(DIR* dd) {
  struct dirent* entry;

  errno = 0;
  entry = readdir(dd);
  while (entry) {
    dir_content.push_back(FileDesc(path, entry));
    entry = readdir(dd);
  }
  if (errno)
    throw(ErrorException(HttpStatus::INTERNAL_SERVER_ERROR));
}
