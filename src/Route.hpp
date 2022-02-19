#pragma once

#include "utils.hpp"
#include "Request.hpp"

class	Route {
	public:
	bool								autoindex;
	std::string							index;
	std::string							root;
	std::map<std::string, std::string>	cgi;
	std::set<std::string>				method;
	std::pair<int, std::string>			redirect;

	Route() {}
	~Route() {}

	void	setAutoIndex(const std::string &_val)
	{ isIn(_val, 4, "true", "1", "false", "0") ? autoindex = strtolower(_val) == "true" || _val == "1" : throw "invalid boolean"; }

	void	setIndex(const std::string &_index)
	{ index = _index; }

	void	setRoot(const std::string &_root)
	{ root = urlsanitizer(replaceAll(_root, "$PWD", cwd())); }

	void	setCgi(const std::string &_ext, const std::string &_path)
	{ cgi[_ext] = _path.substr(_path.find_first_not_of('.')); }

	void	setMethod(const std::string &_method)
	{ isIn(_method, 3, "GET", "POST", "DELETE") ? method.insert(_method) : throw "invalid method"; }

	void	setRedirect(const int _code, const std::string &_redirect)
	{ _code >= 300 && _code <= 307 ? redirect = make_pair(_code, _redirect) : throw "invalid redirect code"; }
	void	setRedirect(const std::string &_code, const std::string &_redirect)
	{ strisdigit(_code) ? setRedirect(atoi(_code.c_str()), _redirect) : throw "invalid unumber"; }

	void	debug() const
	{
		std::cout << "\tautoindex " << autoindex << ENDL;
		std::cout << "\tindex " << index << ENDL;
		std::cout << "\troot " << root << ENDL;
		for (std::map<std::string, std::string>::const_iterator it = cgi.cbegin(); it != cgi.cend(); it++)
			std::cout << "\tcgi " << it->first << " " << it->second << ENDL;
		for (std::set<std::string>::const_iterator it = method.cbegin(); it != method.cend(); it++)
			std::cout << "\tmethod " << *it << ENDL;
		std::cout << "\tredirect " << redirect.first << " " << redirect.second << ENDL;
	}
};