#pragma once

#include "utils.hpp"
#include "Response.hpp"

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
		bool		chunked;

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

			if ((it = headers.find("Transfer-Encoding")) != headers.end())
			{
				std::vector<std::string> fields = split(it->second, ", ");
				if (contains(fields, std::string("chunked")))
					chunked = true;
			}

			appendRaw(data);
		}

		bool decodeChunk(const std::string &chunkedData)
		{
			std::string line;
			std::stringstream	ss(chunkedData);

			std::getline(ss, line);
			size_t chunkSize = strtol(line.c_str(), NULL, 16);
			size_t currChunkSize = 0;
			while (std::getline(ss, line))
			{
				if (chunkSize == 0)
					return true;
				currChunkSize += line.length() + 1;
				if (currChunkSize <= chunkSize)
				{
					line = trim(line, "\r");
					line += "\n";
					raw += line;
				}
				if (currChunkSize == chunkSize && std::getline(ss, line))
				{
					chunkSize = strtol(line.c_str(), NULL, 16);
					currChunkSize = 0;
				}
			}
			return false;
		}

		void appendRaw(const std::string &data)
		{
			if (chunked)
			{
				if (decodeChunk(data))
					ended = true;
				return ;
			}
			remaining -= data.size();
			raw += data;
			if (remaining <= 0)
				ended = true;
		}
		Content() : remaining(0), multipart(false), ended(false), chunked(false) {}
		~Content() {}
	};
	void addHeader(const std::string &header, const std::set<std::string> &name)
	{
		size_t sep = header.find(":");
		if (sep == std::string::npos) throw 400;
		std::string key = header.substr(0, sep);
		std::string value = header.substr(sep + 1);
		if (strtolower(key) == "host")
		{
			std::string	host = trim(split(value, ":")[0]);
			if (!name.count(host) && host != "localhost" && !isip(host))
				throw 0;
		}
		trim(key, " \n\t\r");
		trim(value, " \n\t\r");
		headers[key] = value;
	}
	void setHeaders(std::stringstream &ss, const std::set<std::string> &name)
	{
		std::string header;
		while (std::getline(ss, header) && !header.empty() && header != "\r")
			addHeader(header, name);
	}
	public:
	std::string	request;
	std::string	type;
	std::string	url;
	std::string	query;
	std::string	protocol;
	int			sock;
	Response	response;

	std::map<std::string, std::string, casecomp> headers;
	Content		content;
	bool		empty;

	void init(const std::string &data, const std::set<std::string> &name)
	{
		std::stringstream	ss(data);
		std::getline(ss, request);
		std::vector<std::string>	req = split(request);
		if (req.size() != 3) throw 400;
		type = req[0];
		url = urlsanitize(req[1]);
		query = getQuery(req[1]);
		protocol = trim(req[2], "\r");
		if (protocol != "HTTP/1.1")
			throw 505;
		if (!isIn(type, 3, "GET", "POST", "DELETE"))
			throw 400;

		setHeaders(ss, name);

		if (ss.tellg() != -1)
			content.init(data.substr(ss.tellg()), headers);

		empty = false;
	}

	void	setSock(int _sock)
	{
		sock = _sock;
	}

	//Request(const std::string &data, const std::set<std::string> &name)
	//{
	//	init(data, name);
	//}

	void addContent(const std::string &raw, const std::set<std::string> &name)
	{
		if (empty)
			init(raw, name);
		else
			content.appendRaw(raw);
		if (content.raw.size() > static_cast<size_t>(((headers.count("Content-Length")) 
			? atoi(headers["Content-Length"].c_str()) : 0)))
				throw 413;
	}



	bool ended() const { return (!empty && content.ended); }
	
	Request() : empty(true) {}
	~Request() {}
};


std::ostream &operator<<(std::ostream &os, const Request &req)
{
	std::string request_method = std::string(req.request);
	os << "METHOD" << trim(request_method, "\r") << std::endl;
	os << "HEADERS [" << std::endl;
	for (std::map<std::string, std::string, casecomp>::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it)
		os << "KEY: (" << it->first <<  ") VALUE: (" << it->second << ")" << std::endl;
	os << "]" << std::endl;
	os << "RECEIVED CONTENT LENGTH: (" << req.content.raw.length() << ")" << std::endl;
	os << "RAW CONTENT: (" << req.content.raw << ")" << std::endl;
	os << "ended: (" << req.ended() << ")" << std::endl;
	os << "empty: (" << req.empty << ")" << std::endl;
	return os;
}
