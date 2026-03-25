#ifndef CONTENT_MAP_HPP
#define CONTENT_MAP_HPP
#include <map>
#include <string>

class ContentMap {
   private:
	std::map<std::string, std::string> _allContentType;

   public:
	ContentMap();
	std::string getContentValue(std::string key);
};

#endif
