#ifndef TCPSOCKET_HPP
#define TCPSOCKET_HPP

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <exception>
#include <string>

#include "Request.hpp"

class TCPSocket {
   public:
	TCPSocket(int p);
	TCPSocket(TCPSocket const& rhs);
	TCPSocket();
	~TCPSocket();
	TCPSocket& operator=(TCPSocket const& rhs);

	int			getPort() const;
	int			getListeningSocketPort() const;
	int			getFd() const;
	bool		getError() const;
	std::string getMessageIn() const;
	std::string getMessageOut() const;
	bool		getKeepAlive() const;
	bool 		getListening() const;
	Server* 	getDefaultServer() const;
	std::vector<Server>& getServers();
	void		getRawData(std::string& content, int len);
	std::string getLine();
	void		setMessageIn(std::string msg);
	void		setMessageOut(std::string msg);
	void		setError(bool er);
	void		setKeepAlive(bool keep);
	void		setServers(std::vector<Server> serv);
	void		addRawData(std::string& content, int len);
	void		deleteRequest();
	TCPSocket* 	accept() const;
	int			send();
	void		close();
	int			readAll();

	Request* req;

	class SocketException : public std::exception {
	   public:
		virtual const char* what() const throw() { return ("Error: socket failed"); }
	};

   private:
	void	setDefaultServer();

	int				   socket_fd;
	int				   listening_port;
	struct sockaddr_in socket_addr;
	socklen_t		   socket_addr_length;
	std::string		   msg_in;
	std::string		   msg_out;
	bool				keep_alive;
	bool				error;
	bool				listening;
	Server*				default_server;
	std::vector<Server> servers;

	static const int	backlog;
	static const size_t buffer_size;
	static const size_t max_line_len;
};

#endif
