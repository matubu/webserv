#pragma once

#include "utils.hpp"
#include "Request.hpp"

class	Route {
	public:
	bool								autoindex;
	std::string							index;
	std::string							root;
	std::map<std::string, std::string>	cgi;
	std::vector<std::string>			method;
	std::pair<int, std::string>			redirect;

	Route() {}
	~Route() {}

	void	setAutoIndex(const std::string &_val)
	{ isIn(strtolower(_val), 4, "true", "1", "false", "0") ? autoindex = strtolower(_val) == "true" || _val == "1" : throw "invalid boolean"; }

	void	setIndex(const std::string &_index)
	{ index = _index; }

	void	setRoot(const std::string &_root)
	{ root = urlsanitizer(_root); }

	void	setCgi(const std::string &_ext, const std::string &_path)
	{ cgi[_ext] = _path; }

	void	setMethod(const std::string &_method)
	{ isIn(strtolower(_method), 3, "get", "post", "delete") ? method.push_back(strtolower(_method)) : throw "invalid method"; }

	void	setRedirect(const int _code, const std::string &_redirect)
	{ _code >= 300 && _code <= 307 ? redirect = make_pair(_code, _redirect) : throw "invalid redirect code"; }
	void	setRedirect(const std::string &_code, const std::string &_redirect)
	{ strisdigit(_code) ? setRedirect(atoi(_code.c_str()), _redirect) : throw "invalid unumber"; }

	void	debug()
	{
		std::cout << "\tautoindex " << autoindex << ENDL;
		std::cout << "\tindex " << index << ENDL;
		std::cout << "\troot " << root << ENDL;
		for (std::map<std::string, std::string>::iterator it = cgi.begin(); it != cgi.end(); it++)
			std::cout << "\tcgi " << it->first << " " << it->second << ENDL;
		for (size_t i = 0; i < method.size(); i++)
			std::cout << "\tmethod " << method[i] << ENDL;
		std::cout << "\tredirect " << redirect.first << " " << redirect.second << ENDL;
	}
};