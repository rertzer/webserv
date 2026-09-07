#ifndef INCLUDES_WEBSERV_LOCATION_HPP_
#define INCLUDES_WEBSERV_LOCATION_HPP_

#include <functional>
#include <string>
#include <vector>

#include "webserv/BitSet.hpp"
#include "webserv/HttpMethod.hpp"
#include "webserv/HttpStatus.hpp"
#include "webserv/autoindex.hpp"

using LocationParsingHandler = std::function<void(const std::vector<std::string>&)>;

class LineLoc;

class Location {
 private:
  std::string              _locationPath;
  AutoIndex                autoindex;
  std::string              index;
  std::string              root;
  HttpStatus               redirection_status;
  std::string              redirection_path;
  std::vector<std::string> allow_methods;
  std::string              extension;
  std::string              cgi_path;
  std::string              upload_path;
  BitSet                   allowed_method;

  void addLine(std::string ls);
  void setRoot(std::vector<std::string> list);
  void setAutoIndex(std::vector<std::string> list);
  void setIndex(std::vector<std::string> list);
  void setRedirection(std::vector<std::string> list);
  void setExtension(std::vector<std::string> list);
  void setCgiPath(std::vector<std::string> list);
  void setUploadPath(std::vector<std::string> list);
  void setAllowedMethods(std::vector<std::string> list);

 public:
  Location() = default;
  explicit Location(std::vector<std::string> locationStrings);

  std::string getLocationPath() const;
  bool        checkForRedirection();
  std::string getIndex() const;
  std::string getRoot() const;
  HttpStatus  getRedirectionStatus() const;
  std::string getRedirectionPath() const;
  AutoIndex   getAutoindex() const;
  std::string getExtension() const;
  std::string getCgiPath() const;
  std::string getUploadPath() const;
  BitSet      getAllowedMethods() const;
  bool        isAllowed(HttpMethod method) const;
  bool        isRooted() const;
};

#endif  // INCLUDES_WEBSERV_LOCATION_HPP_
