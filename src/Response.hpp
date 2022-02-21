#pragma once

#include "utils.hpp"
#include "HttpCode.hpp"
	
const	std::string g_ferrorpage = ftos(DEFAULT_ERROR_FILE);

const std::string	g_fautoindex = ftos(AUTOINDEX_TEMPLATE_FILE);
const size_t		g_fautoindex_start = g_fautoindex.find("{{") == std::string::npos ? g_fautoindex.size() - 2 : g_fautoindex.find("{{");
const size_t		g_fautoindex_end = g_fautoindex.find("}}") == std::string::npos ? g_fautoindex.size() - 2 : g_fautoindex.find("}}");
const std::string	g_fautoindex_before = g_fautoindex
						.substr(0, g_fautoindex_start);
const std::string	g_fautoindex_repeat = g_fautoindex
						.substr(g_fautoindex_start + 2, g_fautoindex_end - g_fautoindex_start - 2);
const std::string	g_fautoindex_after = g_fautoindex
						.substr(g_fautoindex_end + 2);

std::string headers(int code, size_t len, const std::string &type)
{
	return ("HTTP/1.1 " + httpCodeToString(code) + "\r\nContent-length: " + atos(len) + "\r\nContent-Type: " + type + "\r\n\r\n");
}

void sendfd(int new_sock, int fd)
{
	#ifdef __APPLE__
		struct sf_hdtr	hdtr = { NULL, 0, NULL, 0 };
		off_t len = 0;
		sendfile(fd, new_sock, 0, &len, &hdtr, 0);
	#else
		long int off = 0;
		while (sendfile(new_sock, fd, &off, SENDFILE_BUF))
			;
	#endif
}

class Response
{
	public:
	std::string	header;
	std::string	body;
	int			readfd;
	bool		fullfilled;
	bool		useread;

	Response() : readfd(0), fullfilled(false), useread(false) {}
	~Response() {}
	void	setBody(const std::string &_header, const std::string &_body)
	{
		std::cout << "setbody " << _header << _body << std::endl;
		header = _header;
		body = _body;
		fullfilled = true;
	}
	void	setFd(const std::string &_header, int fd, int _useread = false)
	{
		std::cout << "setbody " << _header << std::endl << fd << std::endl;
		header = _header;
		readfd = fd;
		fullfilled = true;
		useread = _useread;
	}
	void	setError(int code, const std::map<int, std::string> &error)
	{
		struct stat	stats;
		const std::map<int, std::string>::const_iterator	it = error.find(code);

		if (it != error.end() && exist(it->second, &stats))
		{
			setFd(headers(200, stats.st_size, mime(it->second)), open(it->second.c_str(), O_RDONLY));
			return ;
		}
		std::string	file = replaceAll(replaceAll(g_ferrorpage,
					"$NAME", httpCodeToString(code)),
					"$CODE", atos(code));
		setBody(headers(code, file.size(), "text/html"), file);
	}
	void	setRedirect(int code, const std::string &url)
	{
		setBody("HTTP/1.1 " + atos(code) + "\r\n"
				+ "Location: " + url + "\r\n\r\n", "");
	}
	void	setAutoindex(
		const std::map<int, std::string> &error,
		const std::string &url,
		const std::string &path
		)
	{
		DIR				*dir;
		struct dirent	*diread;
		std::string		s = g_fautoindex_before;

		if ((dir = opendir(path.c_str())) == NULL)
			setError(403, error);
		while ((diread = readdir(dir)))
		{
			if (!strcmp(diread->d_name, ".")
				|| (!strcmp(diread->d_name, "..") && url == "/")) continue ;

			struct stat	stats;
			char		date[128];

			stat((path + "/" + diread->d_name).c_str(), &stats);
			strftime(date, 128, "%d %h %Y", localtime(&stats.st_ctime));

			s += replaceAll(replaceAll(replaceAll(replaceAll(replaceAll(g_fautoindex_repeat,
						"$NAME", diread->d_name),
						"$URL", url + diread->d_name),
						"$DATE", std::string(date)),
						"$SIZE", readable_fsize(stats.st_size)),
						"$ISDIR", stats.st_mode & S_IFDIR ? "1" : "");
		}
		closedir(dir);
		s += g_fautoindex_after;
		setBody(headers(200, s.size(), "text/html"), s);
	}

	bool	readFd()
	{
		char	buf[READFILE_BUF + 1];
		int		ret = read(readfd, buf, READFILE_BUF);

		if (ret == -1 || ret == 0)
			return (false);
		buf[ret] = '\0';
		body += buf;
		return (true);
	}
	void	writeSock(int fd)
	{
		std::cout << "sending on fd " << fd << std::endl;
		std::cout << "header: " << header << std::endl;
		std::cout << "readfd: " << readfd << std::endl;
		std::cout << "body: " << body << std::endl;
		send(fd, (header + body).c_str(), (header + body).size(), 0);
		if (readfd && !useread)
		{
			sendfd(fd, readfd);
			close(readfd);
			readfd = 0;
		}
	}
};