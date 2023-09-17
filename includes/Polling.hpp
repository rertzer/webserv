/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polling.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 10:31:16 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/17 16:54:25 by rertzer          ###   ########.fr       */
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
# include <poll.h>

# include "TCPSocket.hpp"
# include "Event.hpp"

class	Polling
{
	public:
		Polling();
		~Polling();

		void		addMotherSocket(int port);
		void		addCgiFds(std::vector<int> fds);
		void		connect(Event const & ev);
		void		removeMotherSocket(int fd);
		void		removeSocket(int fd);
		void		removeCgiFd(int fd);
		int			wait();
		Event		nextEvent();
		TCPSocket *	getSocketByFd(int fd);
		TCPSocket *	getSocketByCgiFd(int fd);
		bool		isMother(Event ev) const;
		void		setOut(int fd);
		void		resetOut(int fd);
		void		reset(int fd);
		void		setCgiIn(TCPSocket * soc);
		void		addCgiFds(TCPSocket * soc);

		class PollingException: public std::exception
		{
			public:
				virtual const char * what() const throw()
				{
					return ("Error: polling error");
				}
		};

	private:
		Polling & operator=(const Polling & rhs);
		Polling(const Polling & rhs);
		void	addSocket(TCPSocket * soc);
		void	addCgiFd(int fd, int events, TCPSocket * soc);
		void	removeFd(int fd);
		TCPSocket * getSocketFromStrip(int fd, std::map<int, TCPSocket *> & strip) const;
		Event	extractEvent(nfds_t i);

		struct pollfd				fds[256];
		nfds_t						nfds;
		int							events_nb;
		std::list<int>				mother_fds;
		std::map<int, TCPSocket *>	powerstrip;
		std::map<int, TCPSocket *>	powerstripCgi;

};

#endif
