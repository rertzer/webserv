/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 11:04:25 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/03 14:58:56 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_HPP
# define EVENT_HPP

# include <iostream>
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
		void		handleEvent();
		void		handleIn();
		void		handleOut();
		void		handleError();

	private:
		Event();

		int			soc_fd;
		int			events;
		TCPSocket *	soc;
		std::vector<Server> serv;

		typedef void	(Event::*handlefun)();
		static const int    ev[7];
};
#endif
