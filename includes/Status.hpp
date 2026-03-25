#ifndef STATUS_HPP
#define STATUS_HPP

#include <string>

class Status {
   public:
	static std::string getReason(int code);
	static std::string getMsg(int code);

   private:
	Status() {};
};

#endif
