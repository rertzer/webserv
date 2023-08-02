/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 11:04:25 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/02 11:46:08 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_HPP
# define EVENT_HPP

# include <iostream>
# include <sys/epoll.h>
# include "TCPSocket.hpp"

typedef void	(*handlefun)();

class	Event
{
	public:
		Event(int sfd, int e);
		Event(Event const & rhs);
		~Event();

		Event &	operator=(Event const & rhs);
		
		int			getSocketFd() const;
		int			getEvents() const;
		bool		isIn();
		bool		isOut();
		bool		isRdhup();
		bool		isPri();
		bool		isErr();
		bool		isHup();
		bool		isEt();
		bool		isOneshot();
		void		handleEvent();
		void		handleIn();
		void		handleOut();
		void		handleError();

	private:
		Event();

		int			soc_fd;
		int			events;

		static const std::vector<int>    ev;

		
};
#endif
