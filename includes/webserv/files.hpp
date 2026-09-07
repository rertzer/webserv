#ifndef INCLUDES_WEBSERV_FILES_HPP_
#define INCLUDES_WEBSERV_FILES_HPP_

#include <string>

bool        isDirectory(std::string fileName);
bool        isRegularFile(std::string fileName);
bool        isReadable(const std::string& path);
bool        isWritable(const std::string& path);
bool        fileExists(const std::string& path);
void        checkRegularReadable(std::string path);
std::string fileExtension(std::string const& filename);
std::string readSpecFile(std::string file);
std::string getFileContent(std::string path);

#endif  // INCLUDES_WEBSERV_FILES_HPP_
