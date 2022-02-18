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
		return envp;
	}
};


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

		gci_env env;
		for (std::map<std::string, std::string, casecomp>::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it)
			env.add_env(it->first, it->second);
		/*
			Php ne trouve plus les fichier REQUEST_METHOD setup
		*/
		//env.add_env("REQUEST_METHOD", req.type);
		env.add_env("SCRIPT_NAME", uri.c_str()); // chmin complet script

		/*
			// Session et Cookie fonctionne avec les valeur setup plus tôt dans le for
			Key actuel Cookie, mais fonctionne ==> Official key HTTP_COOKIE
			HTTP_COOKIE
			Les éventuels cookies. Une liste de paires clef=valeur contenant les cookies positionnés par le site, séparés par des points-virgules.
		*/
		/*
			// POST
			CONTENT_TYPE
			Le type de contenu attaché à la requête, si des données sont attachées (comme lorsqu'un formulaire est envoyé avec la méthode « POST »).
		
			// GET
			QUERY_STRING
			Contient tout ce qui suit le « ? » dans l'URL envoyée par le client. Toutes les variables provenant d'un formulaire envoyé avec la méthode « GET » seront contenues dans le QUERY_STRING sous la forme « var1=val1&var2=val2&... ».
		*/

		dup2(s_cfd[0], 0);
		close(s_cfd[0]);
		dup2(c_sfd[1], 1);
		close(c_sfd[1]);

		char * const argv[] = { const_cast<char *>(cgi.c_str()),
								const_cast<char *>(uri.c_str()), 0 };

		if (execve(cgi.c_str(), argv, env.to_envp()) == -1)
		{
			// syserr("execve() " + cgi);
			write(1, "ERROR", 6);
		}
		exit(0);
	}
	else //server side
	{
		char buf[1024 + 1];

		close(s_cfd[0]);
		close(c_sfd[1]);

		write(s_cfd[1], req.content.raw.c_str(), req.content.raw.length());
		close(s_cfd[1]);

		std::string line = "HTTP/1.1 200 OK\r\n";
		int n;
		while ((n = read(c_sfd[0], buf, 1024)) != 0)
		{
			buf[n] = 0;
			line += std::string(buf);
		}
		if (line == "ERROR" || line.substr(0, 7) == "Status:")
		{
			std::string error = line.substr(8);
			std::vector<std::string> e = split(error, " ");
			errorpage(e[0], e[1], fd);
			return ;
		}
		send(fd, line.c_str(), line.size(), 0);
		close(c_sfd[0]);
	}
}
