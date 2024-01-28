#pragma once

#include "Server.hpp"
#include "utils.hpp"

bool assertArgCount(std::vector<std::string> argv, size_t argc)
{
	if (argv.size() != argc) throw "invalid argument count";
	return (1);
}

class Config {
	public:
	std::vector<Server> servers;

	Config(const char *filename)
	{
		std::ifstream		f;
		std::string			ln;
		int					idx = 0;
		struct stat			stats;

		if (!stat(filename, &stats) && stats.st_mode & S_IFDIR)
			syserr(std::string(filename), "is a directory");
		f.open(filename);
		if (f.fail())
			syserr("cannot open " + std::string(filename));
		while (getline(&idx, f, ln))
		{
			try {
				if (ln != "server")
					throw "invalid property";

				Server server;
				while (scope(&idx, f, ln))
				{
					std::vector<std::string> sargv = split(ln);
					if (sargv[0] == "match" && assertArgCount(sargv, 2))
					{
						Route &route = server.routes[sanitizeUrl(sargv[1])];
						while (scope(&idx, f, ln))
						{
							std::vector<std::string> margv = split(ln);
							if (margv[0] == "autoindex" && assertArgCount(margv, 2))
								route.setAutoIndex(margv[1]);
							else if (margv[0] == "index" && assertArgCount(margv, 2))
								route.setIndex(margv[1]);
							else if (margv[0] == "root" && assertArgCount(margv, 2))
								route.setRoot(margv[1]);
							else if (margv[0] == "cgi" && assertArgCount(margv, 3))
								route.setCgi(margv[1], margv[2]);
							else if (margv[0] == "method" && assertArgCount(margv, 2))
								route.setMethod(margv[1]);
							else if (margv[0] == "redirect" && assertArgCount(margv, 3))
								route.setRedirect(margv[1], margv[2]);
							else
								throw "invalid property";
						}
						if (route.method.empty())
						{
							route.setMethod("GET");
							route.setMethod("POST");
							route.setMethod("DELETE");
						}
					}
					else if (sargv[0] == "port" && assertArgCount(sargv, 2))
						server.setPort(sargv[1]);
					else if (sargv[0] == "host" && assertArgCount(sargv, 2))
						server.setHost(sargv[1]);
					else if (sargv[0] == "name" && assertArgCount(sargv, 2))
						server.setName(sargv[1]);
					else if (sargv[0] == "error" && assertArgCount(sargv, 3))
						server.setError(sargv[1], sargv[2]);
					else if (sargv[0] == "body_size" && assertArgCount(sargv, 2))
						server.setBodySize(sargv[1]);
					else
						throw "invalid property";
				}
				servers.push_back(server);
			}
			catch (const char *e)
			{
				println(2, std::string(RED "error: ") + filename + ":" + atos(idx) + ": " + e);
				exit(1);
			}
		}
		f.close();
	}
	~Config() {}
};