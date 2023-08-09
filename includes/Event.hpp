/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 11:04:25 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/09 10:58:08 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_HPP
# define EVENT_HPP

# include <iostream>
# include <sstream>
# include <map>
# include <vector>
# include <sys/epoll.h>

# include "TCPSocket.hpp"
# include "Request.hpp"
# include "Server.hpp"

class	Event
{
	public:
		Event(int sfd, int e, TCPSocket* soc);
		Event(Event const & rhs);
		~Event();

		Event &	operator=(Event const & rhs);
		
		int			getSocketFd() const;
		TCPSocket *	getSocket() const;
		int			getEvents() const;
		void		setServ(std::vector<Server> s);
		bool		isIn() const;
		bool		isOut() const;
		bool		isRdhup() const;
		bool		isPri() const;
		bool		isErr() const;
		bool		isHup() const;
		bool		isEt() const;
		bool		isOneshot() const;
		int			handleEvent();
		int			handleIn();
		int			handleOut();
		int			handleError();

	private:
		Event();

		int			soc_fd;
		int			events;
		TCPSocket *	soc;
		std::vector<Server> serv;

		typedef int			(Event::*handlefun)();
		static const int    ev[7];
		static const int	line_max;
};
#endif
