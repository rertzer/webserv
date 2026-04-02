#ifndef EVENT_HPP
#define EVENT_HPP

#include <poll.h>
#include <vector>

#include "ErrorException.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "TCPSocket.hpp"
#include "macroDef.hpp"

constexpr size_t MAX_POLL_EVENT = 5;
/* Event status: message for/from Polling
 * 0 = do nothing
 * 1 = set Out on fd
 * 2 = reset Out on fd
 * 3 = close socket fd
 * 4 = cgi init : add Cgi fds (POST)
 * 5 = cgi fd
 * 6 = close Cgi fd
 * 7 = switch Cgi In
 * 8 = add cgi fds and exec (GET)
 * 9 = close cgi fd*/

enum class eventStatus {
	NOTHING,
	IN,
	OUT,
	CLOSE,
	CGI_INIT,
	CGI_CONTINUE,
	CGI_CLOSE,
	CGI_POST_EXEC,
	CGI_GET_EXEC,
	CGI_ERROR
};

class Event {
   public:
	Event(int f, int e, TCPSocket* soc);
	Event(Event const& rhs);
	~Event();

	Event& operator=(Event const& rhs);

	int			getFd() const;
	TCPSocket*	getSocket() const;
	int			getEvents() const;
	eventStatus getStatus() const;
	void		setServ(std::vector<Server> s);
	bool		isIn() const;
	bool		isOut() const;
	bool		isErr() const;
	bool		isHup() const;
	bool		isCgiFd() const;
	void		handleEvent();
	void		handleIn();
	void		handleOut();
	void		handleError();
	void		handleHup();
	void		handleNval();
	void		internalError();
	void		cgiExec();

   private:
	Event();
	void handleCgiIn();
	void handleCgiOut();
	bool cgiIsPending();
	void handleOneEvent(int i);
	void handleErrorException(const ErrorException& e);

	int					fd;
	int					events;
	eventStatus			status;
	TCPSocket*			soc;
	std::vector<Server> servers;

	typedef void (Event::*handlefun)();
	static const int poll_event[MAX_POLL_EVENT];
};
#endif
