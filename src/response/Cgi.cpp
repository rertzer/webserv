/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 15:02:29 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/14 13:39:13 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <fcntl.h>
#include "Cgi.hpp"
#include "Request.hpp"

//public
Cgi::Cgi(std::string m, std::string p, Request & r, std::pair<std::string, std::string> cp):method(m), path(p), buffer(NULL), buffer_size(1600000), status(0), req(r), cgi_path(cp)
{
	setUrl();
	setEnv();
	initPipes();
	if (method == "POST")
		status = 1;
	else
		status = 2;
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

int	Cgi::getStatus() const
{
	return status;
}

std::vector<int>	Cgi::getFds() const
{
	std::vector<int>	fds;
	fds.push_back(post_fd[0]);
	fds.push_back(post_fd[1]);
	fds.push_back(pipe_fd[0]);
	fds.push_back(pipe_fd[1]);
	return fds;
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
	pos = url.rfind(cgi_path.first);

	if (pos != -1)
	{
		path += url.substr(0, pos + cgi_path.first.size());
		if (pos + cgi_path.first.size() + 1 < url.size())
			path_info = url.substr(pos + cgi_path.first.size() + 1, -1);
	}
}

void	Cgi::setEnv()
{
	env_map["REDIRECT_STATUS"] = "200";
	env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
	env_map["REQUEST_METHOD"] =  method;
	env_map["SERVER_PROTOCOL"] = "HTTP/1.1";
	env_map["REQUEST_URI"] = req.getQuery();
	env_map["SCRIPT_FILENAME"] = path;
	env_map["SCRIPT_NAME"] = path;
	env_map["PATH_INFO"] = path_info;
	env_map["PATH_TRANSLATED"] = "";
	env_map["QUERY_STRING"] = query_string;
	env_map["REMOTE_ADDR"] = "127.0.0.1";
	env_map["HTTP_COOKIE"] = req.getField("Cookie");
}

void	Cgi::initPipes()
{
	if (method == "POST")
		setPostFd();
	else
	{
		post_fd[0] = -1;
		post_fd[1] = -1;
	}
	setPipeFd();
}

void	Cgi::setPostFd()
{
	if (::pipe(post_fd) == -1)
		throw (ErrorException(500));
//	int r = fcntl(post_fd[0], F_SETFL, fcntl(post_fd[0], F_GETFL) | O_NONBLOCK);
//	std::cout << "fcntl: " << r << std::endl;
	fcntl(post_fd[1], F_SETFL, fcntl(post_fd[1], F_GETFL) | O_NONBLOCK);
}

void	Cgi::setPipeFd()
{
	if (::pipe(pipe_fd) == -1)
		throw (ErrorException(500));
	fcntl(pipe_fd[0], F_SETFL, fcntl(pipe_fd[0], F_GETFL) | O_NONBLOCK);
}

int	Cgi::writePostFd()
{
	int size = ::write(post_fd[1], req.getContent().c_str(), req.getContent().size());
	if (size < 0)
	{
		perror("pipe error");
		throw (ErrorException(500));
	}
	std::cout << static_cast<unsigned int>(size) << " " << req.getContent().size() <<std::endl;
	if (static_cast<unsigned int>(size) == req.getContent().size())
		status = 3;
	else
		status = 5;
	req.eraseContent(size);

	return size;
}

int	Cgi::readPipeFd()
{
	buffer = new char[buffer_size + 1];
	int	size = ::read(pipe_fd[0], buffer, buffer_size);
	if (size <= 0)
	{
		delete[] buffer;
		::close(pipe_fd[0]);
		status = 4;
		throw (ErrorException(500));
	}
	buffer[size] = '\0';

	std::stringstream ss;
	ss.write(buffer, size);
	delete[] buffer;
	content = ss.str();
	if (size != 65536)
	{
		::close(pipe_fd[0]);
		status = 4;
	}
	return size;
}

void	Cgi::exec()
{
	int	pid = ::fork();
	if (pid < 0)
		throw (ErrorException(500));
	if (pid == 0)
		execSon();
	if (pid)
		execFather();
}

int	Cgi::execSon()
{
		if (method == "POST")
		{
			if (::dup2(post_fd[0], 0) == -1  || ::close(post_fd[0]) == -1 || ::close(post_fd[1]) == -1)
			{
				perror("dup2 or close error");
				exit(-1);
			}
		}
		if (::dup2(pipe_fd[1], 1) == -1 || ::close(pipe_fd[0]) == -1 || ::close(pipe_fd[1]) == -1)
		{
			perror("dup2 or close error");
			exit(-1);
		}
		char** argv = formatArgv();
		char** envp = formatEnv();
		std::cerr << "Executing cgi script " << argv[0] << " " << argv[1] << std::endl;
		::execve(cgi_path.second.c_str(), argv, envp);
		delete[] argv;
		delete[] envp;
		exit(-1);
}

void	Cgi::execFather()
{
	::close(pipe_fd[1]);
	if (status == 2)
		status = 3;
}

char **	Cgi::formatArgv() const
{
	char ** argv = new  char *[3];
	argv[0] = strdup(cgi_path.second.c_str());
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
