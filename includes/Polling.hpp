/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polling.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 10:31:16 by rertzer           #+#    #+#             */
/*   Updated: 2023/07/29 11:27:52 by rertzer          ###   ########.fr       */
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
		void	addSocket(TCPSocket & sock);
		void	removeSocket(TCPSocket & sock);
		int		wait();
		Event	getEvent(int n) const;
		Event	getNextEvent() const;
	private:
		Polling(const Polling & rhs){};
		Polling & operator=(const Polling & rhs){};

		int	epoll_fd;
		map	powerstrip;

};

#endif
