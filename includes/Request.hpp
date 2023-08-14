/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:06:50 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/14 09:16:56 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <map>
# include <string>
# include <iostream>
# include <sstream>
# include <exception>

# include "macroDef.hpp"
# include "TCPSocket.hpp"
# include "Status.hpp"
# include "ErrorException.hpp"

class Request
{
	public:
		Request(TCPSocket * s);
		Request(Request const & rhs);
		~Request();

		Request &	operator=(Request const & rhs);

		int												getPort() const;
		int												getStatus() const;
		const std::string &								getProtocol() const;
		const std::string &								getMethod() const;
		const std::string &								getQuery() const;
		const std::map<std::string, std::string> &		getHeader() const;
		const std::map<std::string, std::string> &		getTrailer() const;
		const std::stringstream &						getContent() const;
		std::string										getField(std::string const & name) const;
		bool											checkField(std::string const & name, std::string const & value) const;
		int												getIntField(std::string const & name) const;
		void											addField(std::string const & field);
	private:
		//Request();
		void	setControlData();
		void	setHeader();
		void	setContent();
		void	setContentByChunked();
		void	setContentByLength(int len);
		void	checkControlData() const;
		void	checkHeader() const;
		bool	contentExist() const;

		class	RequestException: public std::exception
		{
			public:
				virtual const char * what() const throw()
				{
					return ("Error: request parsing error");
				}
		};
		int									port;
		int									status;
		TCPSocket *							soc;
		std::map<std::string, std::string>	header;
		std::map<std::string, std::string>	trailer;
		std::string							method;
		std::string							query;
		std::string							protocol;
		std::stringstream					content;

		static const int	line_max;
};

std::ostream &	operator<<(std::ostream & ost, Request const & rhs);

#endif
