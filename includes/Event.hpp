#ifndef EVENT_HPP
#define EVENT_HPP

#include <poll.h>

#include "ErrorException.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "Connection.hpp"
#include "macroDef.hpp"

class Polling;

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
	Event();
	Event(Event const& rhs);
	virtual ~Event();

	Event& operator=(Event const& rhs);

	int				getFd() const;
	Connection*		getSocket() const;
	int				getEvents() const;
	eventStatus 	getStatus() const;
	void			setFd(int fd);
	void			setEvents(int events);
	void			setPool(Polling* pool);
	void			setSoc(Connection* soc);
	void			handleEvent();

   protected:
	bool			isIn() const;
	bool			isOut() const;
	bool			isErr() const;
	bool			isHup() const;
	virtual void	handleIn() = 0;
	virtual void	handleOut() = 0;
	virtual void	handleError() = 0;
	virtual void	handleHup() = 0;
	virtual void	handleNval() = 0;
	void	 		handleOneEvent(int i);
	virtual void	handleErrorException(const ErrorException& e) = 0;
	virtual void	handleEventStatus() = 0;

	int					fd;
	int					events;
	eventStatus			status;
	Polling*			pool;
	Connection*			soc;

	typedef void (Event::*handlefun)();
	static const int poll_event[MAX_POLL_EVENT];
};
#endif
