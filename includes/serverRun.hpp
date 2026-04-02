#ifndef SERVERRUN_HPP
#define SERVERRUN_HPP

#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

#include "Polling.hpp"
#include "Server.hpp"

typedef void (*handlestatus)(Event& ev, Polling& pool);

statusCode serverRun(std::vector<Server>);
void	   handleEvent(Polling& pool, std::vector<Server>& serv);
void	   loadListeningSocket(Polling& pool, std::vector<Server> serv);
void	   eventOnListeningSocket(Event& ev, Polling& pool);
void	   eventOnOther(Event& ev, Polling& pool);
void	   checkBadEventOnListeningSocket(Event& ev, Polling& pool);
void	   handleEventStatus(Event& ev, Polling& pool);
void	   handleInOk(Event& ev, Polling& pool);
void	   handleOutOk(Event& ev, Polling& pool);
void	   handleClose(Event& ev, Polling& pool);
void	   handleCgiPostStart(Event& ev, Polling& pool);
void	   handleCgiContinue(Event& ev, Polling& pool);
void	   handleCgiEnd(Event& ev, Polling& pool);
void	   handleCgiPostExec(Event& ev, Polling& pool);
void	   handleCgiGetExec(Event& ev, Polling& pool);
void	   handleCgiError(Event& ev, Polling& pool);

#endif
