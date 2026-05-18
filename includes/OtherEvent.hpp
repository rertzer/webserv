#ifndef OTHEREVENT_HPP
#define OTHEREVENT_HPP

#include <poll.h>

#include "ErrorException.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "TCPSocket.hpp"
#include "macroDef.hpp"
#include "Event.hpp"

class OtherEvent: public Event {
   public:
	OtherEvent();
	OtherEvent(OtherEvent const& rhs);
	~OtherEvent();

	OtherEvent& operator=(Event const& rhs);

   private:
	bool		isCgiFd() const;
	bool		isCgiStatus(CgiStatus cgi_status) const;
	void		handleIn();
	void		handleOut();
	void		handleError();
	void		handleHup();
	void		handleNval();
	void		internalError();
	void		cgiExec();

	void	 handleCgiIn();
	void	 handleCgiOut();
	void	 handleMessageOut();
	bool	 cgiIsPending();
	void	 handleOneEvent(int i);
	void	 handleErrorException(const ErrorException& e);
	void	 handleEventStatus();
	bool	 checkAndHandleCgiIn();
	void	 handleInRequestReady();
	Response getListeningSocketResponse();
	Server*	 getListentingSocketServer();
	void	handleInOk();
	void	handleOutOk();
	void	handleClose();
	void	handleCgiPostStart();
	void	handleCgiContinue();
	void	handleCgiEnd();
	void	handleCgiPostExec();
	void	handleCgiGetExec();
	void	handleCgiError();

	typedef void (OtherEvent::*handlestatus)();
};
#endif
