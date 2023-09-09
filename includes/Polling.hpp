/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polling.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 10:31:16 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/09 11:40:51 by rertzer          ###   ########.fr       */
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
		int			wait();
		Event		nextEvent();
		TCPSocket *	getSocketByFd(int fd);
		bool		isMother(Event ev) const;
		void		setOut(int fd);
		void		resetOut(int fd);
		void		reset(int fd);

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
		void	compressFds();


		struct pollfd				fds[256];
		nfds_t						nfds;
		int							events_nb;
		std::list<int>				mother_fds;
		std::map<int, TCPSocket *>	powerstrip;
		std::map<int, TCPSocket *>	powerstripCgi;

};

#endif
