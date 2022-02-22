#pragma once

#include "utils.hpp"

class HttpCode {
	public:
	std::map<int, std::string> code;

	HttpCode() {
		code[100] = "Continue";
		code[101] = "Switching Protocols";
		code[200] = "OK";
		code[201] = "Created";
		code[202] = "Accepted";
		code[203] = "Non-Authoritative Information";
		code[204] = "No Content";
		code[205] = "Reset Content";
		code[206] = "Partial Content";
		code[300] = "Multiple Choices";
		code[301] = "Moved Permanently";
		code[302] = "Found";
		code[303] = "See Other";
		code[304] = "Not Modified";
		code[305] = "Use Proxy";
		code[307] = "Temporary Redirect";
		code[400] = "Bad Request";
		code[401] = "Unauthorized";
		code[402] = "Payment Required";
		code[403] = "Forbidden";
		code[404] = "Not Found";
		code[405] = "Method Not Allowed";
		code[406] = "Not Acceptable";
		code[407] = "Proxy Authentication Required";
		code[408] = "Request Time-out";
		code[409] = "Conflict";
		code[410] = "Gone";
		code[411] = "Length Required";
		code[412] = "Precondition Failed";
		code[413] = "Request Entity Too Large";
		code[414] = "Request-URI Too Large";
		code[415] = "Unsupported Media Type";
		code[416] = "Requested range not satisfiable";
		code[417] = "Expectation Failed";
		code[500] = "Internal Server Error";
		code[501] = "Not Implemented";
		code[502] = "Bad Gateway";
		code[503] = "Service Unavailable";
		code[504] = "Gateway Time-out";
		code[505] = "HTTP Version not supported";
	}
	std::string getError(int err) const
	{
		return (code.find(err)->second);
	}
	~HttpCode() {}
};

const HttpCode httpCode;

std::string httpCodeToString(int code)
{
	return (atos(code) + " " + httpCode.getError(code));
}
