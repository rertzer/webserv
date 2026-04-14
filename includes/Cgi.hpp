#ifndef CGI_HPP
#define CGI_HPP

#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

#include "Server.hpp"

class Request;
/*Cgi status :
 * 0 = not initialized
 * 1 = waiting to write post_fd
 * 2 = ready to exec
 * 3 = waiting to read pipe_fd
 * 4 = cgi done
 * 5 = post to read*/

enum class CgiStatus { NO_INIT, WAIT_WRITE_POST, READY_EXEC, WAIT_READ_PIPE, DONE, POST_TO_READ };

class Cgi {
   public:
	Cgi(std::string p, Request& r, std::string extension, std::string cgi_path);
	Cgi(Cgi const& rhs);
	~Cgi();

	Cgi& operator=(Cgi const& rhs);

	std::string		 getPath() const;
	std::string		 getContent() const;
	CgiStatus		 getStatus() const;
	std::vector<int> getFds() const;
	int				 getPid() const;
	int				 writePostFd();
	int				 readPipeFd();
	void			 closePipe();
	void			 stop();
	void			 exec();

	class CgiException : public std::exception {
	   public:
		virtual const char* what() const throw() { return ("Error: cgi problem"); }
	};

   private:
	void   setUrl();
	void   extractQueryString(std::string& url);
	void   setPath(std::string const& url);
	void   setEnv();
	void   initPipes();
	void   setPostFd();
	void   setPipeFd();
	int	   execSon();
	void   execFather();
	char** formatArgv() const;
	char** formatEnv() const;
	size_t appendEnv(char**								env_array,
					 std::map<std::string, std::string> env_map,
					 bool								format = false) const;

	HttpMethod						   method;
	std::string						   path;
	std::string						   working_dir;
	std::string						   path_info;
	std::string						   query_string;
	std::string						   content;
	std::map<std::string, std::string> env_map;
	char*							   buffer;
	int								   buffer_size;
	int								   post_fd[2];
	int								   pipe_fd[2];
	int								   pid;
	CgiStatus						   status;
	Request&						   req;
	std::string						   extension;
	std::string						   cgi_path;
};

#endif
