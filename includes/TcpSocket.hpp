#ifndef TCPSOCKET_HPP
#define TCPSOCKET_HPP

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <exception>
#include <string>

class TcpSocket {
public:
	TcpSocket();
	TcpSocket(int port);
	TcpSocket(TcpSocket const& rhs);
	~TcpSocket();
	TcpSocket& operator=(TcpSocket const & rhs);

	int getPort() const;
	int getFd() const;
	void accept(TcpSocket& csoc) const;
	void close();
	int readAll(std::string& msg) const;
	int send(std::string& msg) const;

	class SocketException : public std::exception {
	   public:
		virtual const char* what() const throw() { return ("Error: socket failed"); }
	};
private:
	int				   socket_fd;
	struct sockaddr_in socket_addr;
	socklen_t		   socket_addr_length;


	static const int	backlog;
	static const size_t buffer_size;
};

#endif
