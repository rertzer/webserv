#ifndef LISTENINGEVENT_HPP
#define LISTENINGEVENT_HPP

#include <poll.h>
#include <string>

#include "Event.hpp"

class Polling;

class ListeningEvent : public Event{
   public:
	ListeningEvent();
	ListeningEvent(Event const& rhs);
	~ListeningEvent();

	ListeningEvent& operator=(ListeningEvent const& rhs);
	void			handleEvent();

   private:
	void	handleIn();
	void	handleOut();
	void	handleError();
	void	handleHup();
	void	handleNval();
	void	internalError(std::string message);
	void	handleErrorException(const ErrorException& e);
	void	handleEventStatus();

};
#endif
