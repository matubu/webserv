#pragma once

#include "utils.hpp"

//bad idea
class Context {
	public:
	std::string	plain;
	bool		full;
	Context() : full(false) {}
	~Context() {}
	Context &operator+=(const std::string &buf)
	{
		if (full)
			return (*this);
		plain += buf;
		full = true;
		return (*this);
	}
};
