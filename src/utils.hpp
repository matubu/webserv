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
#include <sys/ioctl.h>
#include <sys/select.h>
#include <math.h>
#include "mime.hpp"
#include "HttpCode.hpp"

#ifdef __APPLE__
# include <sys/socket.h>
#else
# include <sys/sendfile.h>
#endif

#define DEFAULT_CONF "default.webc"
#define DEFAULT_ERROR_FILE "www/error.html"
#define AUTOINDEX_TEMPLATE_FILE "www/autoindex.html"
#define SENDFILE_BUF 2048
#define READFILE_BUF 2048
#define MAX_CONNECTIONS 256

#define EOC "\033[0m"    //reset
#define ENDL EOC "\n"    //reset + endl
#define RED "\033[1;91m" //red
#define ORA "\033[1;33m" //orange
#define BLU "\033[1;94m" //blue
#define GRE "\033[0;92m" //green
#define GRA "\033[0;90m" //grey

std::string	&trim(std::string &s, const std::string &charset = " \t")
{ return (s.erase(0, s.find_first_not_of(charset)).erase(s.find_last_not_of(charset) + 1)); }

bool	startwith(const std::string &s, const std::string &start)
{ return (s.substr(0, start.size()) == start); }
bool	endwith(const std::string &s, const std::string &end)
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

/* Function to write system error such as open ... */
int	syserr(const std::string &msg, const std::string &reason = std::strerror(errno))
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

bool	scope(int *idx, std::ifstream &f, std::string &ln)
{
	if (!getline(idx, f, ln)) throw "unexpected eof";
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

std::string	strtoupper(std::string str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); it++)
		*it = toupper(*it);
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

std::string    getQuery(const std::string &url)
{
    std::vector<std::string> tmp = split(url, "?");
    return (tmp.size() == 1) ? std::string() : tmp.back();
}

bool	exist(const std::string &name, struct stat *stats)
{ return (stat(name.c_str(), stats) == 0); }

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
	ss << roundf(fsize * 10) / 10 << units[i];
	return (ss.str());
}

std::string headers(const std::string &code, size_t len, const std::string &type)
{
	return ("HTTP/1.1 " + code + "\r\nContent-length: " + atos(len) + "\r\nContent-Type: " + type + "\r\n\r\n");
}

void sendf(int new_sock, const std::string &path, struct stat &stats)
{
	std::string header = headers("200 OK", stats.st_size, mime(path));
	send(new_sock, header.c_str(), header.size(), 0);
	int fd = open(path.c_str(), O_RDONLY);
	#ifdef __APPLE__
		struct sf_hdtr	hdtr = { NULL, 0, NULL, 0 };
		off_t len = 0;
		sendfile(fd, new_sock, 0, &len, &hdtr, 0);
	#else
		long int off = 0;
		while (sendfile(new_sock, fd, &off, SENDFILE_BUF))
			;
	#endif
	close(fd);
}

/* case insensitive string comparison functor */
struct casecomp
{
	bool operator()(const std::string& a, const std::string& b) const
	{
		return strcasecmp(a.c_str(), b.c_str()) < 0;
	}
};

void	redirect(int fd, int code, const std::string &url)
{
	std::string	s = "HTTP/1.1 " + atos(code) + "\r\n";
	s += "Location: " + url + "\r\n\r\n";
	send(fd, s.c_str(), s.size(), 0);
}

std::string	cwd()
{
	char	cwd[PATH_MAX];
	if (getwd(cwd) != NULL)
		return (std::string(cwd));
	return (std::string());
}

template <typename T>
bool contains(const std::vector<T> &v, const T &elem)
{
	if (std::find(v.begin(), v.end(), elem) != v.end())
		return true;
	return false;
}

const	std::string g_ferrorpage = ftos(DEFAULT_ERROR_FILE);

void errorpage(int code, const std::map<int, std::string> &error, int sock)
{
	struct stat	stats;
	const std::map<int, std::string>::const_iterator	it = error.find(code);
	HttpCode HttpCode;

	if (it != error.end() && exist(it->second, &stats))
	{
		sendf(sock, it->second, stats);
		return ;
	}
	std::string	file = replaceAll(replaceAll(g_ferrorpage,
				"$NAME", HttpCode.getError(code)),
				"$CODE", atos(code));
	file = headers(atos(code), file.size(), "text/html") + file;
	send(sock, file.c_str(), file.size(), 0);
}
