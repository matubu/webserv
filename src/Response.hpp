#pragma once

#include "utils.hpp"

class Response
{
	public:
	std::string	header;
	std::string	body;
	int			readfd;

	Response() : readfd(false)
	{}
	~Response() {}
	void	setTxt(const std::string &_header, const std::string &_body)
	{
		header = _header;
		body = _body;
	}
	void	setFd(const std::string &_header, int fd)
	{
		header = _header;
		readfd = fd;
	}
	void	write(int fd)
	{
		send(fd, header.c_str(), header.size(), 0);
		if (readfd)
		{

		}
		else
			send(fd, body.c_str(), body.size(), 0);
	}
};