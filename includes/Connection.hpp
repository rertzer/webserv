#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

#include "Request.hpp"
#include "TcpSocket.hpp"

class Connection {
   public:
	Connection(int p);
	Connection(Connection const& rhs);
	Connection();
	~Connection();
	Connection& operator=(Connection const& rhs);

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
	Connection* 	accept() const;
	int			send();
	void		close();
	int			readAll();

	Request* req;



   private:
	void	setDefaultServer();

	int				   listening_port;
	
	std::string		   msg_in;
	std::string		   msg_out;
	bool				keep_alive;
	bool				error;
	bool				listening;
	Server*				default_server;
	std::vector<Server> servers;
	TcpSocket			soc;

	static const size_t max_line_len;
};

#endif
