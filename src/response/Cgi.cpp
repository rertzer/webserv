/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rertzer <rertzer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 15:02:29 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/16 12:03:12 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

//public
Cgi::Cgi(std::string m, std::string p, Request & r):method(m), path(p), req(r)
{
	setUrl();
	setEnv();
}

Cgi::Cgi(Cgi const & rhs):req(rhs.req)
{
	method = rhs.method;
	path = rhs.path;
	path_info = rhs.path_info;
	query_string = rhs.query_string;
	content = rhs.content;
	env_map = env_map;
}

Cgi::~Cgi()
{}

Cgi &	Cgi::operator=(Cgi const & rhs)
{
	if (this != &rhs)
	{
		method = rhs.method;
		path = rhs.path;
		path_info = rhs.path_info;
		query_string = rhs.query_string;
		req = rhs.req;
		content = rhs.content;
		env_map = env_map;
	}
	return *this;
}

std::string	Cgi::getPath() const
{
	return path;
}

std::string Cgi::getContent() const
{
	return content;
}

void	Cgi::exec()
{
	std::cout << "method: " << method << "\n script: " << script << "\n path: " << path;
	std::cout << "\n path_info: " << path_info << "\n query_string: " << query_string << std::endl;
	
	if (method == "GET")
		execGet();
	else if (method == "POST")
		execPost();
}

//private
void	Cgi::setUrl()
{
	std::string url = req.getQuery();
	std::string	query_string;
	int	pos = url.find('?');
	if (pos != -1)
	{
		query_string = url.substr(0, pos);
		url.erase(0, pos + 1);
	}
	pos = url.rfind(".php");
	if (pos != -1)
	{
		script = "php";
		path += url.substr(0, pos + 4);
		path_info = url.substr(pos + 4, -1);
	}
}

void	Cgi::setEnv()
{
	env_map["PATH_INFO"] = path_info;
	env_map["QUERY_STRING"] = query_string;
}

void	Cgi::execGet(std::string & command)
{
	int	fd[2];
	int	status;

	if (pipe(fd) == -1)
		throw (ErrorException(500));
	int	pid = fork();
	if (pid < 0)
		throw (ErrorException(500));
	if (pid == 0)
	{
		if (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1)
			throw (ErrorException(500));
		char** argv = formatArg();
		char** envp = formatEnv();
		execve(editCommand().c_str(),, envp);
		delete[] argv;
		delete[] envp;
		throw (ErrorException(500));
	}
	if (pid)
	{
		waitpid(pid, &status, 0);
		if (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1)
			throw (ErrorException(500));
	}
}

void	Cgi::execPost(std::string & command)
{}

char**	Cgi::formatArgv()
{
	char** argv = new char*[1];
	argv[0] = new char[path.size()];
	*argv[0] = path.c_str();
	return argv;
}

char**	Cgi::formatEnv()
{
	char** env_array = new char*[env_map.size()];
	int	i = 0;
	for (std::map<std::string, std::string>::const_operator it = env_map.begin(); it != env_map.end(); it++)
	{
		std::string	tmp;
		tmp.append(it->first + "=" + it->second);
		env_array[i] = new char[string.size()];
		*env_array[i] = tmp.c_str();
		i++;
	}
	return env_array;
}

std::string Cgi::editCommand()
{
	if (script == "php")
		return ("php ");
	if (script == "py")
		return ("python3 ");
}

