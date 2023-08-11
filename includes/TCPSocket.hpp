/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPSocket.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 09:58:54 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/10 11:19:37 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TCPSOCKET_HPP
# define TCPSOCKET_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <stdlib.h>
# include <unistd.h>
# include <exception>
# include <sys/socket.h>
# include <netinet/in.h>

# include <string.h>
# include "ErrorException.hpp"

class TCPSocket
{
	public:
		TCPSocket(int p);
		TCPSocket(TCPSocket const & rhs);
		TCPSocket();
		~TCPSocket();
		TCPSocket &	operator=(TCPSocket const & rhs);

		int			getPort() const;
		int			getMotherPort() const;
		int			getFd() const;
		void		accept(TCPSocket * csoc);
		void		close();
		int			read();
		int			rawRead(std::stringstream & content, int len);
		std::string	getMessageIn() const;
		std::string	getMessageOut() const;
		void		setMessageIn(std::string msg);
		void		setMessageOut(std::string msg);
		std::string	readLine();
		int			send();

		
		class	SocketException: public std::exception
		{
			public:
				virtual const char * what() const throw()
				{
					return ("Error: socket failed");
				}
		};

	private:
		int					socket_fd;
		int					mother_port;
		struct sockaddr_in	socket_addr;
		socklen_t			socket_addr_length;
		char				buffer[1025];
		std::string			msg_in;
		std::string			msg_out;

		static const int			backlog;
		static const int			buffer_size;
		static const unsigned int	line_size_max;
};

#endif
