#ifndef FILEDESC_HPP
#define FILEDESC_HPP

#include <dirent.h>
#include <sys/stat.h>
#include <ctime>
#include <string>

class FileDesc {
   public:
	FileDesc(std::string p, struct dirent* sd);
	FileDesc(FileDesc const& rhs);
	~FileDesc();
	FileDesc& operator=(FileDesc const& rhs);

	std::string	 getName() const;
	std::string	 getLastModified() const;
	std::string	 getTypeName() const;
	unsigned int getSize() const;
	bool		 isDirectory() const;

   private:
	FileDesc();

	std::string	 name;
	std::string	 last_modified;
	mode_t		 type;
	unsigned int size;
};

#endif
