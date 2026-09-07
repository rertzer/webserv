#ifndef INCLUDES_WEBSERV_CONTENTMAP_HPP_
#define INCLUDES_WEBSERV_CONTENTMAP_HPP_
#include <map>
#include <string>

class ContentMap {
 private:
  std::map<std::string, std::string> _allContentType;

 public:
  ContentMap();
  std::string getContentValue(std::string key);
};

#endif  // INCLUDES_WEBSERV_CONTENTMAP_HPP_
