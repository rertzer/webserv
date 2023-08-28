/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polling.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 10:31:16 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/28 12:52:33 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLLING_HPP
# define POLLING_HPP

# include <iostream>
# include <stdlib.h>
# include <list>
# include <map>
# include <algorithm>
# include <unistd.h>
# include <sys/epoll.h>

# include "TCPSocket.hpp"
# include "Event.hpp"

class	Polling
{
	public:
		Polling();
		~Polling();

		void	addMotherSocket(int port);
		void	connect(Event const & ev);
		void	removeMotherSocket(int fd);
		void	removeSocket(int fd);
		int		wait();
		Event	getEvent(int n);
		Event	nextEvent();
		TCPSocket *	getSocketByFd(int fd);
		bool	isMother(Event ev) const;

	private:
		Polling & operator=(const Polling & rhs);
		Polling(const Polling & rhs);
		void	addSocket(TCPSocket * soc, int events);

		class PollingException: public std::exception
		{
			public:
				virtual const char * what() const throw()
				{
					return ("Error: polling error");
				}
		};

		int							epoll_fd;
		int							events_nb;
		int							next_event;
		struct epoll_event 			events[42];
		std::list<int>				mother_fds;
		std::map<int, TCPSocket *>	powerstrip;

};

#endif
