/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 15:02:29 by rertzer           #+#    #+#             */
/*   Updated: 2023/08/17 13:59:48 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

//public
Cgi::Cgi(std::string m, std::string p, Request & r):method(m), path(p), buffer(NULL), buffer_size(16000000), req(r)
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
	env_map = rhs.env_map;
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
		env_map = rhs.env_map;
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
	int	pos = url.find('?');
	if (pos != -1)
	{
		query_string = url.substr(pos + 1, -1);
		url.erase(pos, -1);
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
	env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
	env_map["REQUEST_METHOD"] = "GET";
	env_map["SERVER_PROTOCOL"] = "HTTP/1.1";
	env_map["REQUEST_URI"] = req.getQuery();
	env_map["SCRIPT_FILENAME"] = path;
	env_map["SCRIPT_NAME"] = path;
	env_map["PATH_INFO"] = path_info;
	env_map["PATH_TRANSLATED"] = "";
	env_map["QUERY_STRING"] = query_string;
	env_map["REMOTE_ADDR"] = "127.0.0.1";
}

void	Cgi::execGet()
{
	int	fd[2];

	std::cout << "ExecGet\n";
	if (::pipe(fd) == -1)
		throw (ErrorException(500));
	std::cout << "pipe done\n";
	int	pid = ::fork();
	if (pid < 0)
		throw (ErrorException(500));
	if (pid == 0)
		execGetSon(fd);
	if (pid)
		execGetFather(fd, pid);
}

int	Cgi::execGetSon(int* fd)
{
	std::cout << "Son___________________\n";
		if (::dup2(fd[1], 1) == -1 || ::close(fd[0]) == -1 || ::close(fd[1]) == -1)
			exit(-1);
		char** argv = formatArgv();
		char** envp = formatEnv();
		std::cerr << editCommand().c_str() << std::endl;
		int i = 0;
		while (argv[i])
			std::cerr << argv[i++] << std::endl;
		i = 0;
		while (envp[i])
			std::cerr << envp[i++] << std::endl;
		::execve(editCommand().c_str(), argv, envp);
		delete[] argv;
		delete[] envp;
		exit(-1);
}

void	Cgi::execGetFather(int* fd, int pid)
{
	std::cout << "Father___________________________\n";
	int	status;

	::close(fd[1]);

	waitpid(pid, &status, 0);
	if (status == -1)
	{
		std::cout << "Status -1\n";
		::close(fd[0]);
		throw (ErrorException(500));
	}

	buffer = new char[buffer_size + 1];
	int	read_size = ::read(fd[0], buffer, buffer_size);
	if (read_size <= 0)
	{
		delete[] buffer;
		::close(fd[0]);
		std::cout << "Read size is " << read_size << std::endl;
		throw (ErrorException(500));
	}
	buffer[read_size] = '\0';

	std::stringstream ss;
	ss.write(buffer, read_size);
	delete[] buffer;
	content = ss.str();
	::close(fd[0]);
}

void	Cgi::execPost()
{}

char **	Cgi::formatArgv() const
{
	char ** argv = new  char *[3];
	//const char* argv[0] = new const char[path.size()];
	argv[0] = strdup(editCommand().c_str());
	argv[1] = strdup(path.c_str());
	argv[2] = NULL;
	return argv;
}

char**	Cgi::formatEnv() const
{

	int env_size = req.getHeader().size() + env_map.size() + 1;
	char** env_array = new char*[env_size];
	int	i = 0;
	for (std::map<std::string, std::string>::const_iterator it = req.getHeader().begin(); it != req.getHeader().end(); it++)
	{
		std::string tmp;
		tmp.append(envFormat(it->first) + "=" + it->second);
		env_array[i] = strdup(tmp.c_str());
		i++;
	}

	for (std::map<std::string, std::string>::const_iterator it = env_map.begin(); it != env_map.end(); it++)
	{
		std::string	tmp;
		tmp.append(it->first + "=" + it->second);
		env_array[i] = strdup(tmp.c_str());
		i++;
	}
	env_array[i] = NULL;
	return env_array;
}

std::string Cgi::editCommand() const
{
	if (script == "php-cgi")
		return ("/usr/bin/php");
	if (script == "py")
		return ("python3");
	return ("");
}
