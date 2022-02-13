#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <algorithm>
#include "mime.hpp"

#ifdef __APPLE__
# include <sys/socket.h>
#else
# include <sys/sendfile.h>
#endif

#define DEFAULT_CONF "default.conf"
#define DEFAULT_ERROR_FILE "www/error.html"
#define AUTOINDEX_TEMPLATE_FILE "www/autoindex.html"
#define SENDFILE_BUF 2048
#define READFILE_BUF 2048

#define EOC "\033[0m"    //reset
#define ENDL EOC "\n"    //reset + endl
#define RED "\033[1;91m" //red
#define ORA "\033[1;33m" //orange
#define BLU "\033[1;94m" //blue
#define GRE "\033[0;92m" //green
#define GRA "\033[0;90m" //grey

std::string	&trim(std::string &s)
{ return (s.erase(0, s.find_first_not_of(" \t")).erase(s.find_last_not_of(" \t") + 1)); }

bool	startwith(std::string s, std::string start)
{ return (s.substr(0, start.size()) == start); }
bool	endwith(std::string s, std::string end)
{
	if (end.size() > s.size()) return (false);
	return (s.substr(s.size() - end.size()) == end);
}

std::vector<std::string>	split(std::string s, const std::string &charset = "\t\n ")
{
	std::vector<std::string>	split;
	size_t	begin;
	size_t	end;

	while ((begin = s.find_first_not_of(charset)) != std::string::npos)
	{
		s = s.substr(begin);
		end = s.find_first_of(charset);
		if (end == std::string::npos) end = s.size();
		split.push_back(s.substr(0, end));
		s = s.substr(end);
	}
	return (split);
}

template <typename T>
std::string	atos(const T &t)
{
	std::ostringstream ss;
	ss << t;
	return (ss.str());
}

void	println(int fd, const std::string &s)
{
	std::string o = s + ENDL;
	write(fd, o.c_str(), o.size());
}

/* Function to write format error in the config file */
int	err(const char *filename, const int idx, const std::string &msg)
{
	println(2, std::string(RED "error: ") + filename + ":" + atos(idx) + ": " + msg);
	exit(1);
}
/* Function to write system error such as open ... */
int	perr(const std::string &msg, const std::string &reason = std::strerror(errno))
{
	println(2, std::string(RED "error: ") + msg + ": " + reason);
	exit(1);
}

bool	getline(int *idx, std::ifstream &f, std::string &ln)
{
	while (++(*idx))
		if (!std::getline(f, ln))
			return (false);
		else if (trim(ln) != "" && !startwith(ln, "//"))
			break ;
	return (true);
}

bool	scope(const char *filename, int *idx, std::ifstream &f, std::string &ln)
{
	if (!getline(idx, f, ln)) err(filename, *idx, "unexpected eof");
	return (ln != ";");
}

bool	strisdigit(const std::string &str)
{ return (str.find_first_not_of("0123465798") == std::string::npos); }

template<typename E, typename T>
bool	isIn(E elm, size_t n, T first ...)
{
	va_list		args;
	va_start(args, first);
	
	while (n--)
	{
		if (elm == first)
		{
			va_end(args);
			return (true);
		}
		first = va_arg(args, T);
	}
	va_end(args);
	return (false);
}

std::string	strtolower(std::string str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); it++)
		*it = tolower(*it);
	return (str);
}

std::string	urlsanitizer(std::string url)
{
	size_t end = url.find_first_of("#?");
	if (end != std::string::npos) url = url.substr(0, end);
	std::vector<std::string> lst = split(url, "/");
	url = "/";
	for (size_t i = 0; i < lst.size(); i++)
		url += lst[i] + "/";
	return (url);
}

bool	exist(const std::string &name, struct stat *info)
{ return (stat(name.c_str(), info) == 0); }

std::string	popchar(const std::string &s)
{ return (s.substr(0, s.size() - 1)); }

std::string	ftos(const std::string &filename)
{
	std::ifstream	f;
	std::string		s;
	char			buf[READFILE_BUF + 1];
	f.open(filename.c_str());

	while (f)
	{
		f.read(buf, READFILE_BUF);
		buf[f.gcount()] = '\0';
		s += buf;
	}
	return (s);
}

std::string	replaceAll(std::string s, const std::string &substr, const std::string &newSubstr)
{
	size_t	idx = 0;
	while (1)
	{
		size_t	find = s.find(substr, idx);
		if (find == std::string::npos) break;
		s.replace(find, substr.size(), newSubstr);
		idx = find + newSubstr.size();
	}
	return (s);
}

std::string	readable_fsize(size_t size)
{
	int					i = 0;
	std::ostringstream	ss;
	float				fsize = size;
	std::string	units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
	while (fsize > 1024 && ++i)
		fsize /= 1024;
	ss << fsize << units[i];
	return (ss.str());
}

std::string headers(const std::string &code, size_t len, const std::string &type)
{
	return ("HTTP/1.1 " + code + "\r\nContent-length: " + atos(len) + "\r\nContent-Type: " + type + "\r\n\r\n");
}

void errorpage(const std::string &code, const std::string &name, int sock)
{
	std::ifstream			ifs;
	std::string				line, file;

	ifs.open(DEFAULT_ERROR_FILE);
	while (std::getline(ifs, line))
		file += replaceAll(replaceAll(line,
				"$NAME", name),
				"$CODE", code);
	file = headers(code, file.size(), "text/html") + file;
	send(sock, file.c_str(), file.size(), 0);
}

void sendf(int new_sock, const std::string &path, struct stat &info)
{
	std::string header = headers("200 OK", info.st_size, mime(path));
	send(new_sock, header.c_str(), header.size(), 0);
	int fd = open(path.c_str(), O_RDONLY);
	#ifdef __APPLE__
		struct sf_hdtr	hdtr = { NULL, 0, NULL, 0 };
		off_t len = 0;
		sendfile(new_sock, fd, 0, &len, &hdtr, 0);
	#else
		long int off = 0;
		while (sendfile(new_sock, fd, &off, SENDFILE_BUF))
			;
	#endif
	close(fd);
}

std::string whichCgi(std::map<std::string, std::string> cgi, std::string file)
{
	std::map<std::string, std::string>::iterator	it;

	for (it = cgi.begin(); it != cgi.end(); it++)
	{
		if (endwith(file, it->first))
			return (it->second);
	}
	return ("");
}