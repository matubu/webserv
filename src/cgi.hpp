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

class gci_env
{
	public:
	std::vector<char *> v;

	void add_env(std::string key, std::string value)
	{
		key = strtoupper(key);
		key = replaceAll(key, "-", "_");
		if (isIn(key, 5, "ACCEPT", "ACCEPT_LANGUAGE", "USER_AGENT", "COOKIE", "REFERER"))
			key = "HTTP_" + key;
		else if (isIn(key, 1, "HOST"))
			key = "REMOTE_" + key;
		std::string testos = key + "=" + value;
		v.push_back(strdup(testos.c_str()));
	}

	char **to_envp() const
	{
		char **envp = new char*[v.size() + 1];
		size_t i;

		for (i = 0; i < v.size(); ++i)
			envp[i] = v[i];
		envp[i] = 0;
		return (envp);
	}
};

void handleCgi(
	const std::map<int, std::string> &error,
	Request &req,
	const std::string &uri,
	const std::string &cgi,
	const std::string &path_info
	)
{
	int request_fd[2], response_fd[2];

	pipe(response_fd);

	int	pid = fork();
	if (pid == 0)
	{
		pipe(request_fd);

		write(request_fd[1], req.content.raw.c_str(), req.content.raw.length());

		close(request_fd[1]);
		close(response_fd[0]);

		gci_env env;
		for (std::map<std::string, std::string, casecomp>::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it)
			env.add_env(it->first, it->second);

		env.add_env("REQUEST_METHOD", req.type);
		env.add_env("PATH_TRANSLATED", uri);
		env.add_env("SCRIPT_NAME", req.url);
		env.add_env("QUERY_STRING", req.query);
		env.add_env("PATH_INFO", path_info);

		dup2(request_fd[0], 0);
		close(request_fd[0]);
		dup2(response_fd[1], 1);
		close(response_fd[1]);

		char *argv[] = { const_cast<char *>(cgi.c_str()),
								const_cast<char *>(uri.c_str()), 0 };

		execve(cgi.c_str(), argv, env.to_envp());
		close(0);
		close(1);
		exit(1);
	}
	else
	{
		close(response_fd[1]);

		int	status;
		waitpid(pid, &status, 0);//TODO nowait
		if (WEXITSTATUS(status))
			req.response.setError(500, error);
		else
		{
			fcntl(response_fd[0], F_SETFL, O_NONBLOCK);
			req.response.setFd("HTTP/1.1 200 OK\r\n", response_fd[0], true);
		}
	}
}
