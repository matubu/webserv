#pragma once

#include "utils.hpp"


class Request {
	private:
	class Content
	{
		public:
		std::string	raw;
		void appendRaw(const std::string &data)
		{
			raw += data;
		}
	};
	//TODO security
	void addHeader(const std::string &header)
	{
		size_t sep = header.find(":");
		// if (sep == std::string::npos)
			// error
		std::string key = header.substr(0, sep);
		std::string value = header.substr(sep + 1);
		trim(key, " \n\t\r");
		trim(value, " \n\t\r");
		headers[key] = value;
	}
	void setHeaders(std::stringstream &ss)
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
	Content		content;

	Request(const std::string &data)
	{
		std::stringstream	ss(data);
		std::getline(ss, request);
		std::vector<std::string>	req = split(request);
		if (req.size() != 3) throw std::runtime_error("invalid request");
		type = req[0];
		url = urlsanitizer(req[1]);
		protocol = req[2];

		setHeaders(ss);

 		std::string	body;
		std::getline(ss, body);
		addContent(body);
	}

	void addContent(const std::string &raw)
	{
		content.appendRaw(raw);
	}
	
	Request() {}
	~Request() {}
};


std::ostream &operator<<(std::ostream &os, const Request &req)
{
	os << "HEADERS [" << std::endl;
	for (std::map<std::string, std::string, casecomp>::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it)
		os << "KEY: (" << it->first <<  ") VALUE: (" << it->second << ")"  << std::endl;
	os << "]" << std::endl;
	os << "RECEIVED CONTENT LENGTH: ("  << req.content.raw.length() << ")" << std::endl; 
	return os;
}