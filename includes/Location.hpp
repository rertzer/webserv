#ifndef LOCATION_HPP
#define LOCATION_HPP
#include "LineLoc.hpp"

class LineLoc;

class Location {
   private:
	std::string			 _locationPath;
	std::vector<LineLoc> _locationLine;

   public:
	void printLoc();
	Location();
	std::vector<LineLoc>&	 getLocationLine();
	std::vector<std::string> getIndex();
	std::string				 getLocationPath() const;
	Location(std::vector<std::string> locationStrings);
};

#endif
