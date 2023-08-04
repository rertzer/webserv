/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:06:50 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/03 15:29:42 by pjay             ###   ########.fr       */
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

class Request
{
	public:
		Request(int p, std::string msg);
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
		void											addField(std::string const & field);
		//std::string	getHost() const;
	private:
		//Request();
		void	setControlData(std::string cdata);
		void	setHeader(std::string head);

		class	RequestException: public std::exception
		{
			public:
				virtual const char * what() const throw()
				{
					return ("Error: request parsing error");
				}
		};
		int									port;
		int									status; //
		std::map<std::string, std::string>	header; //
		std::map<std::string, std::string>	trailer; //
		std::string							method;
		std::string							query;
		std::string							protocol; //
		std::stringstream					content; //
};

std::ostream &	operator<<(std::ostream & ost, Request const & rhs);

#endif
