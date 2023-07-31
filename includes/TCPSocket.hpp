/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPSocket.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 09:58:54 by rertzer           #+#    #+#             */
/*   Updated: 2023/07/31 17:24:58 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TCPSOCKET_HPP
# define TCPSOCKET_HPP

# include <iostream>
# include <string>
# include <stdlib.h>
# include <unistd.h>
# include <exception>
# include <sys/socket.h>
# include <netinet/in.h>

class TCPSocket
{
	public:
		TCPSocket(int p);
		TCPSocket(TCPSocket const & rhs);
		TCPSocket();
		~TCPSocket();
		TCPSocket &	operator=(TCPSocket const & rhs);

		int			getPort() const;
		int			getFd() const;
		void		accept(TCPSocket & csoc);
		void		close();
		int			read();
		std::string	getMessage() const;
		int			send(std::string const & msg);

	private:
		
		class	SocketException: public std::exception
		{
			public:
				virtual const char * what() const throw()
				{
					return ("Error: socket failed");
				}
		};

		int					socket_fd;
		struct sockaddr_in	socket_addr;
		socklen_t			socket_addr_length;
		char				buffer[1024];
		std::string			msg;

		static const int	backlog;
		static const int	buffer_size;
};

#endif
