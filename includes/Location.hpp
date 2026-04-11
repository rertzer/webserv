#ifndef LOCATION_HPP
#define LOCATION_HPP
#include "LineLoc.hpp"

class LineLoc;

class Location {
   private:
	std::string			 _locationPath;
	std::vector<LineLoc> _locationLine;
	void				 addLine(std::string ls);

   public:
	Location() = default;
	Location(std::vector<std::string> locationStrings);

	std::vector<LineLoc>&	 getLocationLine();
	std::vector<std::string> getIndex();
	std::string				 getLocationPath() const;
	bool					 checkForRedirection();
	void					 printLoc();
};

#endif
