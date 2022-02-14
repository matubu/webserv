#pragma once

#include "utils.hpp"
#include "Context.hpp"

class Request {
	public:
	std::string	request;
	std::string	type;
	std::string	url;
	std::string	protocol;

	Request(const std::string &data)
	{
		std::stringstream	ss(data);
		std::getline(ss, request);
		std::vector<std::string>	req = split(request);
		if (req.size() != 3) throw std::runtime_error("invalid request");
		type = req[0];
		url = urlsanitizer(req[1]);
		protocol = req[2];
	}
	~Request() {}
};