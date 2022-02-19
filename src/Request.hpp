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
				// std::cout << "Transfer encoding!" << std::endl;
				std::vector<std::string> fields = split(it->second, ", ");
				if (contains(fields, std::string("chunked")))
				{
					chunked = true;
					std::cout << "chunked!" << std::endl;
				}
			}

			appendRaw(data);
		}

		bool decodeChunk(const std::string &chunkedData)
		{
			std::string line;
			std::stringstream	ss(chunkedData);
			size_t length = 0;

			std::getline(ss, line);
			size_t chunkSize = strtol(line.c_str(), NULL, 16);
			size_t currChunkSize = 0;
			while (std::getline(ss, line))
			{
				// std::cout << "chunkSize:" << chunkSize << std::endl;
				if (chunkSize == 0)
					return true;
				currChunkSize += line.length() + 1;
				if (currChunkSize <= chunkSize)
				{
					line = trim(line, "\r");
					line += "\n";
					raw += line;
					// std::cout << "line: " << line << std::endl;
					// std::cout << "currSize: " << currChunkSize << std::endl;
					// std::cout << "" << std::endl;
				}
				if (currChunkSize == chunkSize && std::getline(ss, line))
				{
					chunkSize = strtol(line.c_str(), NULL, 16);
					length += currChunkSize;
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
		while (std::getline(ss, header) && !header.empty() && header != "\r")
			addHeader(header);
	}
	public:
	std::string	request;
	std::string	type;
	std::string	url;
	std::string	query;
	std::string	protocol;

	std::map<std::string, std::string, casecomp> headers;
	Content		content;
	bool		empty;

	void init(const std::string &data)
	{
		empty = false;

		std::stringstream	ss(data);
		std::getline(ss, request);
		std::vector<std::string>	req = split(request);
		if (req.size() != 3) throw std::runtime_error("invalid request");
		type = req[0];
		url = urlsanitizer(req[1]);
		query = getQuery(req[1]);
		protocol = req[2];

		setHeaders(ss);

		if (ss.tellg() != -1)
			content.init(data.substr(ss.tellg()), headers);
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


std::ostream &operator<<(std::ostream &os, const Request &req)
{
	os << "HEADERS [" << std::endl;
	for (std::map<std::string, std::string, casecomp>::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it)
		os << "KEY: (" << it->first <<  ") VALUE: (" << it->second << ")" << std::endl;
	os << "]" << std::endl;
	os << "RECEIVED CONTENT LENGTH: (" << req.content.raw.length() << ")" << std::endl;
	os << "RAW CONTENT: (" << req.content.raw << ")" << std::endl;
	return os;
}
