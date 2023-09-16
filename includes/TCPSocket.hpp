/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPSocket.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 09:58:54 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/16 13:56:58 by rertzer          ###   ########.fr       */
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
# include "Request.hpp"
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
		bool		getError() const;
		void		setError(bool er);
		void		accept(TCPSocket * csoc);
		void		close();
		int			readAll();
		std::string	getMessageIn() const;
		std::string	getMessageOut() const;
		void		setMessageIn(std::string msg);
		void		setMessageOut(std::string msg);
		std::string	getLine();
		void		getRawData(std::string & content, int len);
		void		addRawData(std::string & content, int len);
		bool		getKeepAlive() const;
		void		setKeepAlive(bool k);
		int			send();

		Request *	req;

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
		std::string			msg_in;
		std::string			msg_out;
		bool				keep_alive;
		bool				error;

		static const int			backlog;
		static const int			buffer_size;
};

#endif
