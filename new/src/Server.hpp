#pragma once

#include "Route.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Context.hpp"
#include "utils.hpp"
#include <sys/select.h>

class	Server {
	public:
	int								port;
	in_addr_t						host;
	std::set<std::string>			name;
	std::map<int, std::string>		error;
	size_t							body_size;
	std::map<std::string, Route>	routes;

	Server() : body_size(1024) {}
	~Server() {}

	/*** SETTERS ***/
	void	setPort(const std::string &_port)
	{ strisdigit(_port) ? port = atoi(_port.c_str()) : throw "invalid unumber"; }

	void	setHost(const std::string &_host)
	{ host = inet_addr(_host == "localhost" ? "127.0.0.1" : _host.c_str()); }

	void	setName(const std::string &_name)
	{ name.insert(_name); }

	void	setError(const std::string &_code, const std::string &_url)
	{ strisdigit(_code) ? error[atoi(_code.c_str())] = _url : throw "invalid unumber"; }

	void	setBodySize(const std::string &_body_size)
	{ strisdigit(_body_size) ? body_size = atoi(_body_size.c_str()) : throw "invalid unumber"; }

	/*** DEBUG ***/
	void	info(const std::string &msg) const
	{ println(1, GRE + msg + " " ORA + atos(port)); }

	void	perr(const std::string &msg) const
	{ println(2, RED "error: " ORA + atos(port) + RED " " + msg + ": " + std::strerror(errno)); }

	void	debug()
	{
		std::cout << ENDL;
		std::cout << "port " << port << ENDL;
		std::cout << "host " << (host & 255) << "." << (host >> 8 & 255) << "." << (host >> 16 & 255) << "." << (host >> 24) << ENDL;
		for (std::set<std::string>::iterator it = name.begin(); it != name.end(); it++)
			std::cout << "name " << *it << ENDL;
		for (std::map<int, std::string>::iterator it = error.begin(); it != error.end(); it++)
			std::cout << "error " << it->first << " " << it->second << ENDL;
		std::cout << "body_size " << body_size << ENDL;
		for (std::map<std::string, Route>::iterator it = routes.begin(); it != routes.end(); it++)
		{
			std::cout << "match " << it->first << ENDL;
			it->second.debug();
		}
	}

	Response	match(std::string url)
	{
		struct stat info;

		if (routes.count(url) && exist(routes[url].root + routes[url].index, &info))
			return (Response(200, routes[url].root + routes[url].index, &routes[url]));
		std::string	save;
		do {
			size_t idx = url.find_last_of('/', url.size() - 2);
			save = url.substr(idx, url.size() - idx - 1) + save;
			url = url.substr(0, idx + 1);
			std::cout << url << " " << save << ENDL;
			if (routes.count(url))
			{
				std::string file = popchar(routes[url].root) + save;
				std::cout << file << ENDL;
				if (exist(file + routes[url].index, &info))
					return (Response(200, file + routes[url].index, &routes[url]));
				if (exist(file, &info))
					return (Response(200, file, &routes[url]));
				break ;
			}
		} while (url != "/");
		if (error.count(404) && exist(error[404], &info))
			return (Response(404, error[404], NULL));
		return (Response(404, "", NULL));
	}

	#define SERVER_ERROR(msg) { \
		server->perr(msg); \
		return (NULL); \
	}

	int accept_new_client(int fd)
	{
		/*** ACCEPT ***/
		int				new_sock;
		struct sockaddr	client_addr;
		socklen_t		client_len = sizeof(client_addr);

	
		if ((new_sock = accept(fd, (struct sockaddr *) &client_addr, &client_len)) < 0)
		{
			perr(ENDL "cannot accept client");
			return (-1);
		}
		return (new_sock);

	}

	void respond_client(int fd, const Context &ctx)
	{
		/*** PARSE ***/
		Request req(ctx);

		std::cout << "request: " << ctx.plain << std::endl;

		/*** FINDING ROUTE ***/
		Response res = match(req.url);

		info(RED "route found " + res.path);

		/*** CGI ***/
		bool pascontent = true;
		std::string cgi = whichCgi(res.routes.cgi, res.path);
		if (!cgi.empty())
		{
			int pipefd[2];
			pipe(pipefd);

			if (fork() == 0)
			{
				close(pipefd[0]);

				dup2(pipefd[1], 1);
				dup2(pipefd[1], 2);

				close(pipefd[1]);

				if (execve(cgi, res.path, NULL) == -1)
				{
					webserv::perror("execve()");
					exit(errno);
				}
			}
			else
			{
				char c;

				close(pipefd[1]);
				std::string line;
				std::string send;
				int n = 1;
				while (n)
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
						pascontent = false;
					}
					if (line.empty())
						continue ;
					webserv::log(line);
					send += line;
					line.clear();
				}
				send = headers("200 OK", send.size(), "text/html") + "\r\n" + send;
				send(fd, send.c_str(), send.size(), 0);
			}
		}

		if (!pascontent)
			return ;

		/*** SEND ***/
		struct stat	info;
		if (stat(res.path.c_str(), &info) == -1 || res.path.empty())
			errorpage("404", "Not Found", fd);
		else if (info.st_mode & S_IFDIR)
		{
			if (res.route && res.route->autoindex)
			{
				DIR				*dir;
				struct dirent	*diread;
				std::string		file = ftos(AUTOINDEX_TEMPLATE_FILE);
				size_t			start = file.find("{{"), end = file.find("}}");
				std::string		s = file.substr(0, start);
				std::string		pattern = file.substr(start + 2, end - start - 2);

				if ((dir = opendir(res.path.c_str())) == NULL)
					errorpage("403", "Forbidden", fd);
				while ((diread = readdir(dir)))
				{
					struct stat	info;
					char		date[128];

					if (!strcmp(diread->d_name, ".") || !strcmp(diread->d_name, "..")) continue ;
					stat((res.path + "/" + diread->d_name).c_str(), &info);
					strftime(date, 128, "%d %h %Y", localtime(&info.st_ctime));

					s += replaceAll(replaceAll(replaceAll(replaceAll(replaceAll(pattern,
								"$NAME", diread->d_name),
								"$URL", req.url + diread->d_name),
								"$DATE", std::string(date)),
								"$SIZE", readable_fsize(info.st_size)),
								"$ISDIR", info.st_mode & S_IFDIR ? "1" : "");
				}
				s += file.substr(end + 2);
				s = headers("200 OK", s.size(), "text/html") + "\r\n" + s;
				send(fd, s.c_str(), s.size(), 0);
			}
			else
				errorpage("403", "Forbidden", fd);
		}
		else
			sendf(fd, res.path, info);
	}

	/*** START ***/
	static void	*start(Server *server)
	{
		server->info("starting ...");

		int					sock;
		struct				sockaddr_in	addr;
		int					opt = 1;
		std::map<int, Context>	ctx;
		char	*buf;

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = server->host;
		addr.sin_port = htons(server->port);

		/*** SETUP ***/
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			SERVER_ERROR("cannot create socket");
		// fcntl(sock, F_SETFL, O_NONBLOCK);
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
			SERVER_ERROR("cannot bind");
		if (listen(sock, 10) < 0)
			SERVER_ERROR("cannot listen");

		server->info("started");

		fd_set	read_fds, readable_fds;
		FD_ZERO(&read_fds);
		FD_SET(sock, &read_fds);
		int		max_fd = sock + 1;

		buf = new char[server->body_size + 1];

		while (1) {
			readable_fds = read_fds;

			select(max_fd, &readable_fds, NULL, NULL, NULL);

			for (int fd = 0; fd < max_fd; ++fd)
			{
				if (!FD_ISSET(fd, &readable_fds))
					continue ;
				if (fd == sock)
				{
					int new_client_fd = server->accept_new_client(sock);
					
					max_fd = std::max(max_fd, new_client_fd + 1);
					FD_SET(new_client_fd, &read_fds);
					// printf("server: %d Joined :)\n", new_client_fd);
					// send_message(new_client_fd, &read_fds, "server: welcome :)\n");
				}
				else
				{
					try {
						int ret = recv(fd, buf, server->body_size, 0);
						if (ret == -1)
							throw std::runtime_error("error recv");
						buf[ret] = '\0';
						const Context& cCtx = (ctx[fd] += std::string(buf));
						if (cCtx.ended)
						{
							server->respond_client(fd, cCtx);
							close(fd);
						}
					}
					catch (std::exception &e)
					{
						errorpage("500", "Internal Server Error", fd);
						close(fd);
					}
					// if (something)
					// 	FD_CLR(i, &read_fds);
				}
			}

			/*** CLOSE ***/
			// close(new_sock);
		}
		delete [] buf;
		return (NULL);
	}
};