#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <functional>

#include "BitSet.hpp"
#include "HttpMethod.hpp"
#include "autoindex.hpp"

using LocationParsingHandler = std::function<void(const std::vector<std::string>&)>;

class LineLoc;

class Location {
   private:
	std::string				 _locationPath;
	AutoIndex				 autoindex;
	std::string				 index;
	std::string				 root;
	int						 redirection_status;
	std::string				 redirection_path;
	std::vector<std::string> allow_methods;
	std::string				 extension;
	std::string				 cgi_path;
	std::string				 upload_path;
	BitSet					 allowed_method;

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
	Location(std::vector<std::string> locationStrings);

	std::string getLocationPath() const;
	bool		checkForRedirection();
	std::string getIndex() const;
	std::string getRoot() const;
	int			getRedirectionStatus() const;
	std::string getRedirectionPath() const;
	AutoIndex	getAutoindex() const;
	std::string getExtension() const;
	std::string getCgiPath() const;
	std::string getUploadPath() const;
	BitSet		getAllowedMethods() const;
	bool		isAllowed(HttpMethod method) const;
};

#endif
