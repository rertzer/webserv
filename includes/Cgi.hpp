/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 14:51:50 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/13 14:02:32 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include <cstdlib>
# include <unistd.h>
# include <sys/wait.h>
# include "Request.hpp"

/*Cgi status :
 * 0 = not initialized
 * 1 = waiting to write post_fd
 * 2 = ready to exec
 * 3 = waiting to read pipe_fd
 * 4 = cgi done
 * 5 = post to read*/

class	Cgi
{
	public:
		//Cgi();
		Cgi(std::string m, std::string p, Request & r, std::pair<std::string, std::string> cp);
		Cgi(Cgi const & rhs);
		~Cgi();

		Cgi &	operator=(Cgi const & rhs);

		std::string			getPath() const;
		std::string			getContent() const;
		int					getStatus() const;
		std::vector<int>	getFds() const;
		int					writePostFd();
		int					readPipeFd();
		void				exec();

	private:
		void		setUrl();
		void		setEnv();
		void		initPipes();
		void		setPostFd();
		void		setPipeFd();
		int			execSon();
		void		execFather();
		char **		formatArgv() const;
		char **		formatEnv() const;

		std::string	method;
		std::string	path;
		std::string	path_info;
		std::string	query_string;
		std::string	content;
		std::map<std::string, std::string> env_map;
		char*	buffer;
		int		buffer_size;
		int		post_fd[2];
		int		pipe_fd[2];
		int		status;
		Request & 	req;
		std::pair<std::string, std::string>	cgi_path;

};

#endif
