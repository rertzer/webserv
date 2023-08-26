/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:06:50 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/26 11:58:56 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <map>
# include <vector>
# include <string>
# include <iostream>
# include <sstream>
# include <fstream>
# include <exception>
# include <fcntl.h>

# include "macroDef.hpp"
//# include "TCPSocket.hpp"
# include "Status.hpp"
# include "ErrorException.hpp"
# include "Server.hpp"


class TCPSocket;

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
		const std::string &								getContent() const;
		std::string										getField(std::string const & name) const;
		bool											checkField(std::string const & name, std::string const & value) const;
		bool											checkSubField(std::string const & name, std::string const & value) const;
		bool											isUpload() const;
		void											upload_all();
		int												getIntField(std::string const & name) const;
		void											addField(std::string const & field);
		void										setQuery(std::string const & query);
		bool										ready() const;
		void										feed();
	private:
		//Request();
		std::string	getLine(std::string const & sep);
		std::string	getLine(std::string & data, std::string const & sep);
		std::string	getFileName();
		void		upload(std::string & part);
		void		uploadFile(std::string const & filename, std::string const & part);
		void		checkValidFileName(std::string const & filename) const;
		void	setControlData();
		void	setHeader();
		void	setContent();
		void	setContentByChunked();
		unsigned int		readChunk();
		void	setTrailer();
		void	setContentByLength(unsigned int len);
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
		std::map<std::string, std::string>	multipart;
		std::string							method;
		std::string							query;
		std::string							protocol;
		std::string							content;
		bool								header_ok;
		bool								content_ok;
};

std::ostream &	operator<<(std::ostream & ost, Request const & rhs);

#endif
