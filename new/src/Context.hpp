#pragma once

#include "utils.hpp"

class Context {
	/*
		check for "eof"
	*/
	bool check(const std::string &buf)
	{
		(void) buf;
		return false;
	}

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
