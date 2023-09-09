/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 11:04:25 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/09 15:33:09 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_HPP
# define EVENT_HPP

# include <iostream>
# include <sstream>
# include <map>
# include <vector>
# include <poll.h>

# include "TCPSocket.hpp"
# include "Request.hpp"
# include "Server.hpp"
# include "macroDef.hpp"
# include "Response.hpp"

/* Event status: message for/from Polling
 * 0 = do nothing
 * 1 = set Out on fd
 * 2 = reset Out on fd
 * 3 = close fd
 * 4 = cgi init : add Cgi fds
 * 5 = cgi fd
 * 6 = close Cgi fd
 * 7 = switch Cgi In*/

class	Event
{
	public:
		Event(int f, int e, TCPSocket* soc);
		Event(Event const & rhs);
		~Event();

		Event &	operator=(Event const & rhs);

		int			getFd() const;
		TCPSocket *	getSocket() const;
		int			getEvents() const;
		int			getStatus() const;
		void		setServ(std::vector<Server> s);
		bool		isIn() const;
		bool		isOut() const;
		bool		isErr() const;
		bool		isHup() const;
		void		handleEvent();
		void		handleIn();
		void		handleOut();
		void		handleError();
		void		handleHup();
		void		cgiExec();

	private:
		Event();
		void		handleCgiIn();
		void		handleCgiOut();

		int			fd;
		int			events;
		int			status;
		TCPSocket *	soc;
		std::vector<Server> serv;

		typedef void		(Event::*handlefun)();
		static const int    ev[5];
};
#endif
