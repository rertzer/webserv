#include <fcntl.h>
#include <stdio.h>

#include "Cgi.hpp"
#include "ErrorException.hpp"
#include "HttpMethod.hpp"
#include "Request.hpp"

// public
Cgi::Cgi(std::string p, Request& r, std::string ext, std::string cp)
	: method(r.getMethod()),
	  path(p),
	  buffer(nullptr),
	  buffer_size(1600000),
	  pid(0),
	  status(CgiStatus::NO_INIT),
	  req(r),
	  extension(ext),
	  cgi_path(cp) {
	setUrl();
	setEnv();
	initPipes();
	if (method == POST)
		status = CgiStatus::WAIT_WRITE_POST;
	else
		status = CgiStatus::READY_EXEC;
}

Cgi::Cgi(Cgi const& rhs) : req(rhs.req) {
	*this = rhs;
}

Cgi::~Cgi() {}

Cgi& Cgi::operator=(Cgi const& rhs) {
	if (this != &rhs) {
		method = rhs.method;
		path = rhs.path;
		working_dir = rhs.working_dir;
		path_info = rhs.path_info;
		query_string = rhs.query_string;
		content = rhs.content;
		env_map = rhs.env_map;
		buffer_size = rhs.buffer_size;
		post_fd[0] = rhs.post_fd[0];
		post_fd[1] = rhs.post_fd[1];
		pipe_fd[0] = rhs.pipe_fd[1];
		pid = rhs.pid;
		status = rhs.status;
		req = rhs.req;
		extension = rhs.extension;
	}
	return *this;
}

std::string Cgi::getPath() const {
	return path;
}

std::string Cgi::getContent() const {
	return content;
}

CgiStatus Cgi::getStatus() const {
	return status;
}

std::vector<int> Cgi::getFds() const {
	std::vector<int> fds;
	fds.push_back(post_fd[0]);
	fds.push_back(post_fd[1]);
	fds.push_back(pipe_fd[0]);
	fds.push_back(pipe_fd[1]);
	return fds;
}

int Cgi::getPid() const {
	return pid;
}

void Cgi::stop() {
	if (kill(pid, SIGKILL) == -1) {
		perror("cgi kill TERM");
		throw(CgiException());
	}
}

// private
void Cgi::setUrl() {
	std::string url = req.getQuery();

	extractQueryString(url);
	setPath(url);
}

void Cgi::extractQueryString(std::string& url) {
	auto pos = url.find('?');
	if (pos != std::string::npos) {
		query_string = url.substr(pos + 1);
		url.erase(pos);
	}
}

void Cgi::setPath(std::string const& url) {
	auto pos = url.rfind(extension);

	if (pos != std::string::npos) {
		path += url.substr(0, pos + extension.size());
		if (pos + extension.size() + 1 < url.size())
			path_info = url.substr(pos + extension.size() + 1);
		pos = path.rfind("/");
		if (pos != std::string::npos)
			working_dir = path.substr(0, pos);
	}
}

void Cgi::setEnv() {
	env_map["REDIRECT_STATUS"] = "200";
	env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
	env_map["REQUEST_METHOD"] = methodToString(method);
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

void Cgi::initPipes() {
	if (method == POST)
		setPostFd();
	else {
		post_fd[0] = -1;
		post_fd[1] = -1;
	}
	setPipeFd();
}

void Cgi::setPostFd() {
	if (::pipe(post_fd) == -1)
		throw(ErrorException(500));
	fcntl(post_fd[1], F_SETFL, fcntl(post_fd[1], F_GETFL) | O_NONBLOCK);
}

void Cgi::setPipeFd() {
	if (::pipe(pipe_fd) == -1)
		throw(ErrorException(500));
	fcntl(pipe_fd[0], F_SETFL, fcntl(pipe_fd[0], F_GETFL) | O_NONBLOCK);
}

int Cgi::writePostFd() {
	ssize_t size = 0;
	size_t	content_size = req.getContent().size();
	if (content_size) {
		size = ::write(post_fd[1], req.getContent().c_str(), content_size);
		if (size <= 0) {
			perror("pipe error");
			::close(post_fd[1]);
			throw(ErrorException(500));
		}
	}
	if (static_cast<size_t>(size) == content_size) {
		status = CgiStatus::WAIT_READ_PIPE;
		::close(post_fd[1]);
	} else {
		status = CgiStatus::POST_TO_READ;
	}
	req.eraseContent(size);

	return size;
}

int Cgi::readPipeFd() {
	buffer = new char[buffer_size + 1];
	int size = ::read(pipe_fd[0], buffer, buffer_size);

	if (size > 0) {
		buffer[size] = '\0';
		content.insert(0, buffer, size);
	} else {
		closePipe();
	}
	delete[] buffer;
	if (size < 0) {
		throw(ErrorException(500));
	}
	return size;
}

void Cgi::closePipe() {
	if (pid)
		stop();
	int ret;
	::waitpid(pid, &ret, 0);
	::close(pipe_fd[0]);
	pid = 0;
	status = CgiStatus::DONE;
}

void Cgi::exec() {
	pid = ::fork();
	if (pid < 0) {
		throw(ErrorException(500));
	} else if (pid == 0) {
		execSon();
	} else {
		execFather();
	}
}

int Cgi::execSon() {
	if (method == POST) {
		if (::dup2(post_fd[0], 0) == -1 || ::close(post_fd[0]) == -1 || ::close(post_fd[1]) == -1) {
			perror("POST: dup2 or close error");
			exit(-1);
		}
	}
	if (::dup2(pipe_fd[1], 1) == -1 || ::close(pipe_fd[0]) == -1 || ::close(pipe_fd[1]) == -1) {
		perror("dup2 or close error");
		exit(-1);
	}
	char** argv = formatArgv();
	char** envp = formatEnv();
	if (::chdir(working_dir.c_str()) != 0) {
		perror("chdir");
		exit(-1);
	}
	::execve(cgi_path.c_str(), argv, envp);
	delete[] argv;
	delete[] envp;
	exit(-1);
}

void Cgi::execFather() {
	::close(pipe_fd[1]);
	if (method == POST)
		::close(post_fd[0]);
	if (status == CgiStatus::READY_EXEC)
		status = CgiStatus::WAIT_READ_PIPE;
}

char** Cgi::formatArgv() const {
	return new char*[3]{strdup(cgi_path.c_str()), strdup(path.c_str()), nullptr};
}

char** Cgi::formatEnv() const {
	int	   env_size = req.getHeader().size() + env_map.size() + 1;
	char** env_array = new char*[env_size];
	size_t i = appendEnv(env_array, req.getHeader(), true);
	i += appendEnv(&env_array[i], env_map);

	env_array[i] = nullptr;
	return env_array;
}

size_t Cgi::appendEnv(char**							 env_array,
					  std::map<std::string, std::string> env_map,
					  bool								 format) const {
	size_t i = 0;
	for (auto& env : env_map) {
		std::string tmp;
		std::string first = env.first;
		if (format) {
			first = envFormat(first);
		}
		tmp.append(first + "=" + env.second);
		env_array[i] = strdup(tmp.c_str());
		i++;
	}
	return i;
}
