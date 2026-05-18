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

		std::vector<Server>	servers;
		Polling				pool;
		statusCode			status;

};



#endif
