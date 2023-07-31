/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polling.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 10:31:16 by rertzer           #+#    #+#             */
/*   Updated: 2023/07/31 12:30:14 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLLING_HPP
# define POLLING_HPP

# include <iostream>
# include <stdlib.h>
# include <map>
# include <unistd.h>
# include <sys/epoll.h>
# include "TCPSocket.hpp"

class	Polling
{
	public:
		Polling();
		~Polling();
		void	addMotherSocket(TCPSocket &soc);
		void	addCommectSocket(TCPSocket & soc);
		void	removeSocket(TCPSocket & soc);
		int		wait();
		Event	getEvent(int n) const;
		Event	getNextEvent() const;
	private:
		Polling(const Polling & rhs){};
		Polling & operator=(const Polling & rhs){};
		void	addSocket(TCPSocket & soc);

		class PollingException: public std::exception
		{
			public:
				virtual const char * what() const throw()
				{
					return ("Error: polling error");
				}
		}
		
		int						epoll_fd;
		int						events_nb;
		int						next_event;
		struct epoll_event 		events[42];
		map<int, TCPSocket &>	powerstrip;

};

#endif
