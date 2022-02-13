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
	bool		ended;

	Context() : ended(false) {}
	~Context() {}

	Context &operator+=(const std::string &buf)
	{
		if (ended)
			return (*this);
		plain += buf;
		ended = true;
		return (*this);
	}
};