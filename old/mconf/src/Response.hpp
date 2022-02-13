#pragma once

#include "Route.hpp"
#include "utils.hpp"

class Response {
	public:
	int			port;
	std::string	path;
	const Route	*route;

	Response(int _port, std::string _path, const Route *_route)
		: port(_port), path(_path), route(_route) {}
	~Response() {}
};