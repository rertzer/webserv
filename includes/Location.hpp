#ifndef LOCATION_HPP
#define LOCATION_HPP
#include "LineLoc.hpp"

class LineLoc;

class Location {
   private:
	std::string				 _locationPath;
	std::vector<LineLoc>	 _locationLine;
	std::string				 root;
	std::vector<std::string> _return;
	std::string				 autoindex;
	std::vector<std::string> allow_methods;
	std::string				 index;
	std::string				 extension;
	std::string				 cgi_path;
	std::string				 upload_path;

	void addLine(std::string ls);

   public:
	Location() = default;
	Location(std::vector<std::string> locationStrings);

	std::vector<LineLoc>&	 getLocationLine();
	std::vector<std::string> getIndex();
	std::string				 getLocationPath() const;
	bool					 checkForRedirection();
	void					 printLoc();
	std::string				 getRoot() const;
};

#endif
