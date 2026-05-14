#ifndef POLLING_HPP
#define POLLING_HPP

#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <list>
#include <map>

#include "Event.hpp"
#include "TCPSocket.hpp"

class Polling {
   public:
	Polling();
	~Polling();

	void	   addListeningSocket(TCPSocket* soc);
	void	   addCgiFds(std::vector<int> fds);
	void	   removeListeningSocket(int fd);
	void	   removeSocket(int fd);
	void	   removeCgiFd(int fd);
	int		   wait();
	Event	   nextEvent();
	TCPSocket* getSocketByFd(int fd);
	TCPSocket* getSocketByCgiFd(int fd);
	void	   setOut(int fd);
	void	   resetOut(int fd);
	void	   reset(int fd);
	void	   setCgiIn(TCPSocket* soc);
	void	   addCgiFds(TCPSocket* soc);
	void	   addSocket(TCPSocket* soc);

	class PollingException : public std::exception {
	   public:
		virtual const char* what() const throw() { return ("Error: polling error"); }
	};

   private:
	Polling& operator=(const Polling& rhs);
	Polling(const Polling& rhs);
	void	   addCgiFd(int fd, int events, TCPSocket* soc);
	void	   removeFd(int fd);
	TCPSocket* getSocketFromStrip(int fd, std::map<int, TCPSocket*>& strip) const;
	Event	   extractEvent(nfds_t i);

	struct pollfd			  fds[256];
	nfds_t					  nfds;
	int						  events_nb;
	std::list<int>			  listening_fds;
	std::map<int, TCPSocket*> powerstrip;
	std::map<int, TCPSocket*> powerstripCgi;
};

#endif
