#pragma once

#include "utils.hpp"

std::string findCgi(const std::map<std::string, std::string> &cgi, const std::string &file)
{
	std::map<std::string, std::string>::const_iterator	it;

	for (it = cgi.begin(); it != cgi.end(); it++)
		if (endwith(file, it->first))
			return (it->second);
	return ("");
}

// run cgi function

void handleCgi(int fd, const Request &req, const std::string &uri ,const std::string &cgi)
{
	int s_cfd[2]; //s_c == server to cgi
	int c_sfd[2]; //c_s == cgi to server
	pipe(s_cfd);
	pipe(c_sfd);
	if (fork() == 0) //cgi side
	{
		close(s_cfd[1]);
		close(c_sfd[0]);

		dup2(s_cfd[0], 0);
		close(s_cfd[0]);
		dup2(c_sfd[1], 1);
		close(c_sfd[1]);

		char * const argv[] = { const_cast<char *>(cgi.c_str()), const_cast<char *>(uri.c_str()), 0 };

		if (execve(cgi.c_str(), argv, NULL) == -1)
		{
			// syserr("execve() " + cgi);
			write(1, "ERROR", 6);
		}
		exit(0);
	}
	else //server side
	{
		char c;

		close(s_cfd[0]);
		close(c_sfd[1]);

		write(s_cfd[1], req.content.raw.c_str(), req.content.raw.length());
		close(s_cfd[1]);

		std::string line;
		std::string s = "HTTP/1.1 200 OK\r\n";
		int n;
		do
		{
			while ((n = read(c_sfd[0], &c, 1)) != 0)
			{
				line += c;
				if (c == '\n')
					break ;
			}
			if (line == "ERROR" || line.substr(0, 7) == "Status:")
			{
				std::string error = line.substr(8);
				std::vector<std::string> e = split(error, " ");
				errorpage(e[0], e[1], fd);
				return ;
			}
			if (line.empty())
				continue ;
			s += line;
			line.clear();
		} while (n);
		send(fd, s.c_str(), s.size(), 0);
		close(c_sfd[0]);
	}
}
