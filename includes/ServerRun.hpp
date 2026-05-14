#ifndef SERVERRUN_HPP
#define SERVERRUN_HPP

#include <vector>

#include "Server.hpp"
#include "Polling.hpp"


class ServerRun{
	public:
		ServerRun(std::vector<Server> servers);
		statusCode run();

	private:

		void	   handleEvent();
		void	   loadListeningSocket();
		void	   eventOnListeningSocket(Event& ev);
		void	   eventOnOther(Event& ev);
		void	   checkBadEventOnListeningSocket(Event& ev);
		void	   handleEventStatus(Event& ev);
		void	   handleInOk(Event& ev);
		void	   handleOutOk(Event& ev);
		void	   handleClose(Event& ev);
		void	   handleCgiPostStart(Event& ev);
		void	   handleCgiContinue(Event& ev);
		void	   handleCgiEnd(Event& ev);
		void	   handleCgiPostExec(Event& ev);
		void	   handleCgiGetExec(Event& ev);
		void	   handleCgiError(Event& ev);

		std::vector<Server>	servers;
		Polling				pool;
		statusCode			status;

};


typedef void (ServerRun::*handlestatus)(Event& ev);

#endif
