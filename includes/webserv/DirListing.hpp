#ifndef INCLUDES_WEBSERV_DIRLISTING_HPP_
#define INCLUDES_WEBSERV_DIRLISTING_HPP_

#include <dirent.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include "webserv/FileDesc.hpp"

class DirListing {
 public:
  explicit DirListing(std::string path);
  DirListing(DirListing const& rhs);
  ~DirListing();
  DirListing& operator=(DirListing const& rhs);

  std::vector<FileDesc> getDirContent() const;

 private:
  DirListing();

  void setDirContent(DIR* dd);

  std::string           path;
  std::vector<FileDesc> dir_content;
};

#endif  // INCLUDES_WEBSERV_DIRLISTING_HPP_
