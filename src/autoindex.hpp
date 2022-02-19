#pragma once

#include "utils.hpp"
#include "Request.hpp"

const std::string	g_fautoindex = ftos(AUTOINDEX_TEMPLATE_FILE);
const size_t		g_fautoindex_start = g_fautoindex.find("{{") == std::string::npos ? g_fautoindex.size() - 2 : g_fautoindex.find("{{");
const size_t		g_fautoindex_end = g_fautoindex.find("}}") == std::string::npos ? g_fautoindex.size() - 2 : g_fautoindex.find("}}");
const std::string	g_fautoindex_before = g_fautoindex
						.substr(0, g_fautoindex_start);
const std::string	g_fautoindex_repeat = g_fautoindex
						.substr(g_fautoindex_start + 2, g_fautoindex_end - g_fautoindex_start - 2);
const std::string	g_fautoindex_after = g_fautoindex
						.substr(g_fautoindex_end + 2);

void	autoindex(int fd, const std::map<int, std::string> &error, const Request &req, const std::string &path)
{
	DIR				*dir;
	struct dirent	*diread;
	std::string		s = g_fautoindex_before;

	if ((dir = opendir(path.c_str())) == NULL)
		errorpage(403, error, fd);
	while ((diread = readdir(dir)))
	{
		if (!strcmp(diread->d_name, ".")
			|| (!strcmp(diread->d_name, "..") && req.url == "/")) continue ;

		struct stat	stats;
		char		date[128];

		stat((path + "/" + diread->d_name).c_str(), &stats);
		strftime(date, 128, "%d %h %Y", localtime(&stats.st_ctime));

		s += replaceAll(replaceAll(replaceAll(replaceAll(replaceAll(g_fautoindex_repeat,
					"$NAME", diread->d_name),
					"$URL", req.url + diread->d_name),
					"$DATE", std::string(date)),
					"$SIZE", readable_fsize(stats.st_size)),
					"$ISDIR", stats.st_mode & S_IFDIR ? "1" : "");
	}
	s += g_fautoindex_after;
	s = headers("200 OK", s.size(), "text/html") + "\r\n" + s;
	send(fd, s.c_str(), s.size(), 0);
}
