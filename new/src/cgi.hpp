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

void    handleCgi(int fd, const std::string &path, const std::string &cgi)
{
	std::cout << "cgi" << ENDL;
	int pipefd[2];
	pipe(pipefd);

	if (fork() == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1], 1);
		dup2(pipefd[1], 2);

		close(pipefd[1]);

		char    **tab = (char **)calloc(sizeof(char *), 3);
		tab[0] = strdup(cgi.c_str());
		tab[1] = strdup(path.c_str());
		if (execve(cgi.c_str(), tab, NULL) == -1)
			syserr("execve() " + cgi);
		exit(0);
	}
	else
	{
		char    c;

		close(pipefd[1]);
		std::string line;
		std::string s =  "HTTP/1.1 200 OK\r\n";
		int n;
		do
		{
			while ((n = read(pipefd[0], &c, 1)) != 0)
			{
				line += c;
				if (c == '\n')
					break ;
			}
			if (line.substr(0, 7) == "Status:")
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
	}
}
