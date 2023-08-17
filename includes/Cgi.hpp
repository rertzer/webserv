/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 14:51:50 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/17 09:41:39 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <string>
# include <map>
# include <cstdlib>
# include <unistd.h>
# include <sys/wait.h>
# include "Request.hpp"

class	Cgi
{
	public:
		//Cgi();
		Cgi(std::string m, std::string p, Request & r);
		Cgi(Cgi const & rhs);
		~Cgi();

		Cgi &	operator=(Cgi const & rhs);

		std::string	getPath() const;
		std::string	getContent() const;
		void		exec();

	private:
		void		setUrl();
		void		setEnv();
		void		execGet();
		int			execGetSon(int* fd);
		void		execGetFather(int* fd, int pid);
		void		execPost();
		char **		formatArgv() const;
		char**		formatEnv() const;
		std::string	editCommand() const;

		std::string	method;
		std::string	script;
		std::string	path;
		std::string	path_info;
		std::string	query_string;
		std::string	content;
		std::map<std::string, std::string> env_map;
		char*	buffer;
		int		buffer_size;
		Request & 	req;

};

#endif
