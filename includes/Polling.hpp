#ifndef POLLING_HPP
#define POLLING_HPP

#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <list>
#include <map>

#include "Event.hpp"
#include "Connection.hpp"

class Polling {
   public:
	Polling();
	~Polling();

	void	   addListeningSocket(Connection* soc);
	void	   addCgiFds(std::vector<int> fds);
	void	   removeListeningSocket(int fd);
	void	   removeSocket(int fd);
	void	   removeCgiFd(int fd);
	int		   wait();
	Event*	   nextEvent();
	Connection* getSocket(nfds_t i) const;
	Connection* getSocketByFd(int fd) const;
	Connection* getSocketByCgiFd(int fd) const;
	void	   setOut(int fd);
	void	   resetOut(int fd);
	void	   reset(int fd);
	void	   setCgiIn(Connection* soc);
	void	   addCgiFds(Connection* soc);
	void	   addSocket(Connection* soc);

	class PollingException : public std::exception {
	   public:
		virtual const char* what() const throw() { return ("Error: polling error"); }
	};

   private:
	Polling& operator=(const Polling& rhs);
	Polling(const Polling& rhs);
	void	   addCgiFd(int fd, int events, Connection* soc);
	void	   removeFd(int fd);
	Connection* getSocketFromStrip(int fd,  const std::map<int, Connection*>& strip) const;
	Event*	   extractEvent(nfds_t i);

	struct pollfd			  fds[256];
	nfds_t					  nfds;
	int						  events_nb;
	std::list<int>			  listening_fds;
	std::map<int, Connection*> powerstrip;
	std::map<int, Connection*> powerstripCgi;
};

#endif
