#ifndef INCLUDES_WEBSERV_FILEDESC_HPP_
#define INCLUDES_WEBSERV_FILEDESC_HPP_
#include <dirent.h>
#include <sys/stat.h>
#include <string>

class FileDesc {
 public:
  FileDesc(std::string p, struct dirent* sd);
  FileDesc(FileDesc const& rhs);
  ~FileDesc();
  FileDesc& operator=(FileDesc const& rhs);

  std::string  getName() const;
  std::string  getLastModified() const;
  std::string  getTypeName() const;
  unsigned int getSize() const;
  bool         isDirectory() const;

 private:
  FileDesc();
  void setTime(struct stat stat_buffer);

  std::string  name;
  std::string  last_modified;
  mode_t       type;
  unsigned int size;
};

#endif  // INCLUDES_WEBSERV_FILEDESC_HPP_
