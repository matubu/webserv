#pragma once

#include "utils.hpp"


class Request {
	private:
	class Content
	{
		public:
		std::string	raw;
		int			remaining;
		bool		multipart;
		std::string boundary;
		bool		ended;
		void init(const std::string &data, const std::map<std::string, std::string, casecomp> &headers)
		{
			std::map<std::string, std::string>::const_iterator it;

			if ((it = headers.find("Content-Length")) != headers.end()) remaining = atoi(it->second.c_str());

			if ((it = headers.find("Content-Type")) != headers.end())
			{
				std::vector<std::string> fields = split(it->second, ";");
				if (fields.size() == 2 && fields[0] == "multipart/form-data")
				{
					multipart = true;
					trim(fields[1], " \n\t\r");
					if (startwith(fields[1], "boundary="))
						boundary = fields[1].substr(9);
				}
			}

			appendRaw(data);
		}
		void appendRaw(const std::string &data)
		{
			//std::cout << "REM" << remaining << std::endl;

			remaining -= data.size();
			raw += data;
			//std::cout << data << std::endl;

			//std::cout << "REM" << remaining << std::endl;
			if (remaining <= 0)
				ended = true;
		}
		Content() : remaining(0), multipart(false), ended(false) {}
		~Content() {}
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
	bool		empty;

	void init(const std::string &data)
	{
		empty = false;
		//std::cout << data << std::endl;

		std::stringstream	ss(data);
		std::getline(ss, request);
		std::vector<std::string>	req = split(request);
		if (req.size() != 3) throw std::runtime_error("invalid request");
		type = req[0];
		url = urlsanitizer(req[1]);
		protocol = req[2];

		setHeaders(ss);

		std::string body;
		if (ss.tellg() != -1)
			body = data.substr(ss.tellg());
		content.init(body, headers);
	}

	Request(const std::string &data)
	{
		init(data);
	}

	void addContent(const std::string &raw)
	{
		if (empty)
			init(raw);
		else
			content.appendRaw(raw);
	}

	bool ended() const { return content.ended; }
	
	Request() : empty(true) {}
	~Request() {}
};

/*
std::ostream &operator<<(std::ostream &os, const Request &req)
{
	os << "HEADERS [" << std::endl;
	for (std::map<std::string, std::string, casecomp>::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it)
		os << "KEY: (" << it->first <<  ") VALUE: (" << it->second << ")"  << std::endl;
	os << "]" << std::endl;
	os << "RECEIVED CONTENT LENGTH: ("  << req.content.raw.length() << ")" << std::endl; 
	return os;
}
*/