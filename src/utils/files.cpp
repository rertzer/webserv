#include <filesystem>
#include <fstream>
#include <sstream>

#include "webserv/ErrorException.hpp"
#include "webserv/HttpStatus.hpp"
#include "webserv/files.hpp"

namespace fs = std::filesystem;

std::string readSpecFile(std::string file) {
  std::ifstream     fileOp;
  std::stringstream fileStr;

  checkRegularReadable(file);
  fileOp.open(file.c_str());

  if (fileOp.is_open()) {
    fileStr << fileOp.rdbuf();
    fileOp.close();
  } else {
    throw(ErrorException(HttpStatus::NOT_FOUND));
  }
  return fileStr.str();
}

bool isDirectory(std::string path) {
  return fs::is_directory(path);
}

bool isRegularFile(std::string path) {
  return fs::is_regular_file(path);
}
bool isReadable(const std::string& path) {
  auto p = fs::status(path).permissions();
  return (p & fs::perms::owner_read) != fs::perms::none;
}

bool isWritable(const std::string& path) {
  auto p = fs::status(path).permissions();
  return (p & fs::perms::owner_write) != fs::perms::none;
}

bool fileExists(const std::string& path) {
  return fs::exists(path);
}

void checkRegularReadable(std::string path) {
  if (!isRegularFile(path)) {
    throw(ErrorException(HttpStatus::NOT_FOUND));
  }
  if (!isReadable(path)) {
    throw(ErrorException(HttpStatus::FORBIDDEN));
  }
}

std::string fileExtension(std::string const& filename) {
  std::string extension;
  auto        lastdot = filename.rfind(".");
  if (lastdot != std::string::npos) {
    extension = filename.substr(lastdot + 1);
  }
  return extension;
}

std::string getFileContent(std::string path) {
  std::ifstream file(path, std::ios::binary);

  if (!file)
    throw std::runtime_error("Cannot open file: " + path);

  file.seekg(0, std::ios::end);
  size_t size = file.tellg();
  file.seekg(0);

  std::string content(size, '\0');
  file.read(content.data(), size);

  return content;
}

std::string extractDirPath(std::string path) {
  if (path != "/") {
    auto pos = path.rfind(".");
    if (pos != std::string::npos) {
      path = path.substr(0, path.rfind("."));
    }
    pos = path.rfind("/");
    if (pos != std::string::npos) {
      path = path.substr(0, path.rfind("/"));
    }
  }
  return path;
}
