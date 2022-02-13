#pragma once

#include "utils.hpp"

class Request {
	public:
	std::string	plain;

	std::string	request;
	std::string	type;
	std::string	url;
	std::string	protocol;

	Request(int sock, size_t body_size)
	{
		char	*buf = new char[body_size + 1];
		int		ret = recv(sock, buf, body_size, 0);
		if (ret == -1) throw "cannot recv";

		buf[ret] = '\0';
		plain = std::string(buf);

		std::stringstream	ss(plain);
		std::getline(ss, request);
		std::vector<std::string>	req = split(request);
		if (req.size() != 3) throw std::runtime_error("invalid request");
		type = req[0];
		url = urlsanitizer(req[1]);
		protocol = req[2];

		delete [] buf;
	}
	~Request() {}
};