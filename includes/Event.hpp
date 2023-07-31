/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 11:04:25 by rertzer           #+#    #+#             */
/*   Updated: 2023/07/31 12:13:16 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_HPP
# define EVENT_HPP

# include <iostream>
# include "Polling.hpp"

class	Event
{
	public:
		Event(TCPSocket s, int e);
		~Event();

		TCPSocket &	getSocket();
		int			getEvents();
		bool		isIn();
		bool		isOut();

	private:
		Event(){};
		Event &	operator=(Event const & rhs){};

		TCPScocket	soc;
		int			events;
		
};
#endif
