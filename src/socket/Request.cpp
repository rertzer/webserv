/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 17:15:31 by rertzer           #+#    #+#             */
/*   Updated: 2023/09/13 16:53:28 by rertzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "TCPSocket.hpp"

Request::Request(TCPSocket * s, std::vector<Server> & serv): port(s->getMotherPort()), status(100), body_size(1000000), soc(s), cgi(NULL), header_ok(false), content_ok(false)

{
	int len = soc->readAll();
	std::cout << "on fd " << soc->getFd() << ", " <<  len << " octets read\n";
	std::cout <<"cgi is " << cgi << std::endl;
	std::cout <<"cgi is " << getCgiStatus() << std::endl;
	if (len == 0)
		throw (RequestException());
	setControlData();
	setHeader(serv);
	if (contentExist())
		setContent();
	else
		status = 5;
}

Request::Request(Request const & rhs)
{
	*this = rhs;
}

Request::~Request()
{
	if (cgi != NULL)
	{
		delete cgi;
		cgi = NULL;
	}
}

Request &	Request::operator=(Request const & rhs)
{
	if (this != &rhs)
	{
		port = rhs.port;
		status = rhs.status;
		body_size = rhs.body_size;
		soc = rhs.soc;
		cgi = rhs.cgi;
		header = rhs.header;
		trailer = rhs.trailer;
		multipart = rhs.multipart;
		query = rhs.query;
		method = rhs.method;
		content = rhs.content;
		header_ok = rhs.header_ok;
		content_ok = rhs.content_ok;
	}
	return *this;
}

int	Request::getPort() const
{
	return port;
}

int	Request::getStatus() const
{
	return status;
}

int	Request::getCgiStatus() const
{
	std::cout << "CGI: " << cgi << std::endl;
	if (cgi != NULL)
		return cgi->getStatus();
	else
		return 0;
}

const std::string	& Request::getProtocol() const
{
	return protocol;
}

const std::string &	Request::getMethod() const
{
	return method;
}

const std::string &	Request::getQuery() const
{
	return query;
}

std::string	Request::getField(std::string const & name) const
{
	std::map<std::string, std::string>::const_iterator it = header.find(name);
	if (it == header.end())
	{
		it = trailer.find(name);
		if (it == trailer.end())
			return "";
	}
	return it->second;
}

unsigned int	Request::getBodySize() const
{
	return body_size;
}

Cgi *	Request::getCgi() const
{
	return cgi;
}

void	Request::setBodySize(int bs)
{
	if (bs > 0)
		body_size = bs * 1000000;
}

void	Request::setUploadPath(std::string up)
{
	upload_path = up;
}

bool	Request::checkField(std::string const & name, std::string const & value) const
{
	std::string field = getField(name);
	std::vector<std::string> all_values = splitCsv(field);
	for (size_t i = 0; i < all_values.size(); i++)
	{
		if (ciCompare(all_values[i], value))
			return true;
	}
	return false;
}

bool	Request::checkSubField(std::string const & name, std::string const & value) const
{
	std::string field = getField(name);
	std::vector<std::string> all_values = splitCsv(field, ";");
	for (size_t i = 0; i < all_values.size(); i++)
	{
		if (ciCompare(all_values[i], value))
			return true;
	}
	return false;
}

bool	Request::isUpload() const
{
	if (getMethod() == "POST" && checkSubField("Content-Type", "multipart/form-data") && ! upload_path.empty())
		return true;
	return false;
}

void	Request::upload_all()
{
	std::string boundary = getLine("\r\n");
	std::string part = getLine(boundary);
	multipart.clear();
	upload(part);
}

void	Request::upload(std::string & part)
{
	std::string line = getLine(part, "\r\n");
	while (line.length())
	{
		int	k = line.find(":");
		if (k == -1 || k == 0)
			throw (ErrorException(400));
		std::string	key = line.substr(0, k);
		std::string	val = line.substr(k + 1);
		stringTrim(val);
		stringTrim(key);
		multipart[key] = val;
		line = getLine(part, "\r\n");
	}
	std::string filename = getFileName();
	if (! filename.empty())
		uploadFile(filename, part);
}

std::string	Request::getFileName()
{
	std::string fn = multipart["Content-Disposition"];
	std::vector<std::string> fields = splitCsv(fn, ";");
	for (std::vector<std::string>::iterator it = fields.begin(); it != fields.end(); it++)
	{
		int	k = it->find("=");
		if (k == -1 || k == 0)
			continue;
		std::string	key = it->substr(0, k);
		std::string	val = it->substr(k + 1);
		stringDoubleQuotTrim(val);
		stringDoubleQuotTrim(key);
		if (key == "filename")
		{
			fn = val;
			break;
		}
	}
	return fn;
}

std::string	Request::getExtension() const
{
	std::string	ext;
	int	begin = query.find(".");
	if (begin == -1)
		return ext;
	ext = query.substr(begin);
	int end = ext.find("?");
	if (end == -1)
		end = ext.find("/");
	if (end != -1)
		ext.erase(end, -1);
	return ext;
}

void	Request::uploadFile(std::string const & filename, std::string const & part)
{
		checkValidFileName(filename);
		std::string path = upload_path + filename;
		if (access(path.c_str(), F_OK) == 0)
			std::cout << "File " << path << " already exist\n";
		else
		{
			std::ofstream upfile(path.c_str(), std::ofstream::out);
			if (upfile.fail())
			{
				std::cout << "File " << filename;
				perror(" creation failed");
				throw (ErrorException(500));
			}
			upfile.write(part.c_str(), part.length());
			upfile.close();
			std::cout << "File " << filename << "created\n";
		}


}

void	Request::checkValidFileName(std::string const & filename) const
{
	if (filename.size() > 255 ||
			filename.find_first_of("\\\0") != std::string::npos ||
			filename == "." ||
			filename == "..")
		throw ErrorException(400);
}


std::string	Request::getLine(std::string const & sep)
{
	int	pos = -1;
	std::string	line;

	pos = content.find(sep);
	if (pos != -1)
	{
		line = content.substr(0, pos);
		content.erase(0, pos + sep.length());
	}
	return (line);
}


bool	Request::ready() const
{
	return header_ok && ((contentExist() && content_ok) || !contentExist());
}


void	Request::feed(std::vector<Server> serv)
{
	int len = soc->readAll();
	if (len == 0)
		throw (RequestException());
	if (!header_ok)
		setHeader(serv);
	if (header_ok && contentExist() && !content_ok)
		setContent();
}

void	Request::eraseContent(int size)
{
	content.erase(0, size);
}


TCPSocket * Request::getSocket() const
{
	return soc;
}

std::string Request::getLine(std::string & data, std::string const & sep)
{
	int	pos = -1;
	std::string	line;

	pos = data.find(sep);
	if (pos != -1)
	{
		line = data.substr(0, pos);
		data.erase(0, pos + sep.length());
	}
	return (line);

}

unsigned int	Request::getUIntField(std::string const & name) const
{
	std::stringstream ss;
	ss << getField(name);
	unsigned int val = 0;
	ss >> val;
	return val;
}

const std::map<std::string, std::string> &	Request::getHeader() const
{
	return header;
}

const std::map<std::string, std::string> &	Request::getTrailer() const
{
	return trailer;
}

const std::string &	Request::getContent() const
{
	return content;
}

void	Request::addField(std::string const & field)
{
	int	k = field.find(":");
	if (k == -1 || k == 0)
		throw (ErrorException(400));
	std::string	key = field.substr(0, k);
	std::string	val = field.substr(k + 1);
	stringTrim(val);
	stringTrim(key);
	if (header.find(key) == header.end())
		header[key] = val;
	else
		header[key] += ", " + val;
}

// Private
void	Request::setControlData()
{
	std::string line = soc->getLine();

	if (line.empty())
		line = soc->getLine();

	int	m = line.find(" ");
	if (m == -1)
		throw (ErrorException(400));
	method = line.substr(0, m);

	int	q = line.find(" ", m + 1);
	if (q == -1)
		throw (ErrorException(400));
	query = line.substr(m + 1, q - (m + 1));

	protocol = line.substr(q + 1);

	checkControlData();
}

void	Request::setQuery(std::string const & query)
{
	this->query = query;
}

void	Request::setHeader(std::vector<Server> serv)
{
	setFields();
	Server theserv = findTheServ(*this, serv, soc->getMotherPort());
	setBodySize(theserv.getBodySize());
	checkHeader();
	setKeepAlive();
	header_ok = true;
}

void	Request::setKeepAlive()
{
	std::string keep = getField("Connection");
	if (keep == "keep-alive")
		soc->setKeepAlive(true);
}

void	Request::setCgi(Cgi * c)
{
	cgi = c;
}

void	Request::setFields()
{
	std::string	line = soc->getLine();

	while (line.length())
	{
		if (line.length())
			addField(line);
		line = soc->getLine();
	}
}

void	Request::setContent()
{
	std::string trans_encoding = getField("Transfer-Encoding");

	if (! trans_encoding.empty())
	{
		if (checkField("Transfer-Encoding", "chunked"))
			setContentByChunked();
		else
			throw (ErrorException(501));
	}
	else
	{
		unsigned int len = getUIntField("Content-Length");
		setContentByLength(len);
	}
}

void	Request::setContentByChunked()
{
	int len = 1;
	while (len)
	{
		len = readChunk();
	}
	setTrailer();
}

unsigned int	Request::readChunk()
{
	std::stringstream ss;
	ss << std::hex << soc->getLine();
	unsigned int	size = 0;
	if (ss >> size)
	{
		if (content.size() + size > body_size)
			throw (ErrorException(413));
		soc->addRawData(content, size);
		soc->getLine();
		if (size == 0)
			content_ok = true;
	}
	return size;
}

void	Request::setTrailer()
{
	setFields();
}

void	Request::setContentByLength(unsigned int len)
{
	int remain = len - content.size();
	if (remain > 0)
	{
		soc->addRawData(content, remain);
	}
	if (content.size() ==  len)
		content_ok = true;
}

void	Request::checkControlData() const
{
	if (protocol != "HTTP/1.1")
		throw (ErrorException(505));
	std::vector<std::string> allowed_methods;

	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
	allowed_methods.push_back("DELETE");

	for (unsigned int i = 0; i < allowed_methods.size(); i++)
	{
		if (method == allowed_methods[i])
			return;
	}
	getCgiStatus();
	throw (ErrorException(501));
}

void	Request::checkHeader() const
{
	if (getField("Host").empty())
		throw (ErrorException(400));
	if (getUIntField("Content-Length") > body_size)
		throw (ErrorException(413));
}

bool	Request::contentExist() const
{
	int	content = 0;
	if (! getField("Transfer-Encoding").empty())
		content++;
	if (! getField("Content-Length").empty())
		content++;
	if (content == 2)
		throw (ErrorException(400));
	if (content)
		return true;
	return false;
}
//Non member functions

std::ostream &	operator<<(std::ostream & ost, Request const & rhs)
{
	ost << "port: " << rhs.getPort() << "\n";
	ost << "protocol: " << rhs.getProtocol() << "\n";
	ost << "method: " << rhs.getMethod() << "\n";
	ost << "query: " << rhs.getQuery() << "\n";
	ost << "status: " << rhs.getStatus() << "\n";
	ost << "Body size: " << rhs.getBodySize() << "\n";
	ost << "Header:\n";
	for (std::map<std::string, std::string>::const_iterator it = rhs.getHeader().begin(); it != rhs.getHeader().end(); it++)
		ost << "\t" << it->first << ": " << it->second << "\n";
	ost << "Trailer:\n";
	for (std::map<std::string, std::string>::const_iterator it = rhs.getTrailer().begin(); it != rhs.getTrailer().end(); it++)
		ost << "\t" << it->first << ": " << it->second << "\n";
	ost << "content: " << rhs.getContent() << "\n";
	return ost;
}
