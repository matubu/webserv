#pragma once

#include "utils.hpp"


class Request {
	private:
	//todo security
	void addHeader(const std::string& header)
	{
		size_t sep = header.find(":");
		// if (sep == std::string::npos)
			// error
		std::string key = header.substr(0, sep);
		std::string value = header.substr(sep + 1);
		trim(key);
		trim(value);
		headers[key] = value;
	}
	void addHeaders(std::stringstream &ss)
	{
		std::string header;
		std::getline(ss, header);
		while (!header.empty() && header != "\r")
		{
			addHeader(header);
			std::getline(ss, header);
		}
	}
	public:
	std::string	request;
	std::string	type;
	std::string	url;
	std::string	protocol;

	std::map<std::string, std::string, casecomp> headers;
	std::string	body;

	Request(const std::string &data)
	{
		std::stringstream	ss(data);
		std::getline(ss, request);
		std::vector<std::string>	req = split(request);
		if (req.size() != 3) throw std::runtime_error("invalid request");
		type = req[0];
		url = urlsanitizer(req[1]);
		protocol = req[2];

		addHeaders(ss);
		std::getline(ss, body);
	}
	~Request() {}
};
