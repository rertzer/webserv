/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 11:04:25 by rertzer           #+#    #+#             */
/*   Updated: 2023/07/31 16:53:44 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_HPP
# define EVENT_HPP

# include <iostream>
# include <sys/epoll.h>
# include "TCPSocket.hpp"

class	Event
{
	public:
		Event(int sfd, int e);
		Event(Event const & rhs);
		~Event();

		Event &	operator=(Event const & rhs);
		
		int			getSocketFd() const;
		int			getEvents();
		bool		isIn();
		bool		isOut();

	private:
		Event();

		int			soc_fd;
		int			events;
		
};
#endif
