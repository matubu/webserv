#pragma once

#include "Route.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Context.hpp"
#include "utils.hpp"
#include "autoindex.hpp"
#include "cgi.hpp"

class Server {
	public:
	int								sock;
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
	{ println(1, ORA + atos(port) + " " GRE + msg); }
	void	syserr(const std::string &msg) const
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
		struct stat stats;

		if (routes.count(url) && exist(routes[url].root + routes[url].index, &stats))
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
				if (exist(file + routes[url].index, &stats))
					return (Response(200, file + routes[url].index, &routes[url]));
				if (exist(file, &stats))
					return (Response(200, file, &routes[url]));
				break ;
			}
		} while (url != "/");
		if (error.count(404) && exist(error[404], &stats))
			return (Response(404, error[404], NULL));
		return (Response(404, "", NULL));
	}
/*
	int accept_new_client(int fd)
	{
		int				new_sock;
		struct sockaddr	client_addr;
		socklen_t		client_len = sizeof(client_addr);

	
		if ((new_sock = accept(fd, (struct sockaddr *) &client_addr, &client_len)) == -1)
		{
			if (errno != EAGAIN)
				syserr(ENDL "cannot accept client");
			return (-1);
		}
		info(RED "client accepted");
		fcntl(new_sock, F_SETFL, O_NONBLOCK);
		return (new_sock);
	}
*/
	void respond_client(int fd, const Context &ctx)
	{
		/*** PARSE ***/
		Request req(ctx.plain);

		/*** FINDING ROUTE ***/
		Response res = match(req.url);

		info("route found: " + res.path);

		/*** CGI ***/
		std::string cgi = findCgi(routes["/"].cgi, res.path);
		if (!cgi.empty())
		{
			info("using CGI " + cgi);
			handleCgi(fd, res, cgi);
			return ;
		}

		/*** SEND ***/
		struct stat	stats;
		if (stat(res.path.c_str(), &stats) == -1 || res.path.empty())
		{
			info("404");
			errorpage("404", "Not Found", fd);
		}
		else if (stats.st_mode & S_IFDIR)
		{
			if (res.route && res.route->autoindex)
			{
				info("autoindex");
				autoindex(fd, req, res);
			}
			else
			{
				info("autoindex forbidden");
				errorpage("403", "Forbidden", fd);
			}
		}
		else
		{
			info("sendfile");
			sendf(fd, res.path, stats);
		}
	}

	void	initsocket(int *sock)
	{
		struct	sockaddr_in	addr;

		/*** SETUP ***/
		if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			throw "cannot create socket";

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = host;
		addr.sin_port = htons(port);

		int					on = 1;
		if (setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
			throw "setsockopt failed";

		if (bind(*sock, (struct sockaddr *) &addr, sizeof(addr)) == -1)
			throw "cannot bind";

		if (listen(*sock, MAX_CONNECTIONS) == -1)
			throw "cannot listen";

		//fcntl(*sock, F_SETFL, O_NONBLOCK);
	}

	/*** START ***/
	static void	*start(Server *server)
	{
		server->info("starting ...");

		int						sock, new_sock;
		std::map<int, Context>	ctx;

		try { server->initsocket(&sock); }
		catch (const char *e)
		{
			server->syserr(e);
			return (NULL);
		}

		server->info("started");

   		fd_set	master_set, working_set;
		FD_ZERO(&master_set);
		int		max_fd = sock;
		FD_SET(sock, &master_set);

		char	*buf = new char[server->body_size + 1];

		while (1) {
			working_set = master_set;

			if (select(max_fd + 1, &working_set, NULL, NULL, NULL) == -1)
			{
				server->syserr("cannot select");
				break ;
			}
			server->info("connection ready");
			for (int fd = 0; fd <= max_fd; ++fd)
			{
				if (!FD_ISSET(fd, &working_set))
					continue ;
				if (fd == sock)
				{
					if ((new_sock = accept(fd, NULL, NULL)) == -1)
						server->syserr(ENDL "cannot accept client");
					fcntl(new_sock, F_SETFL, O_NONBLOCK);
					FD_SET(new_sock, &master_set);
					max_fd = std::max(max_fd, new_sock);
					server->info(atos(new_sock) + " accepted");
				}
				else
				{
					try {
						std::cout << "recv()" << std::endl;
						int	rc = recv(fd, buf, server->body_size, 0);
						std::cout << "recv ended" << std::endl;
						if (rc == -1)
						{
							if (errno != EAGAIN && errno != EWOULDBLOCK)
								throw std::runtime_error("error recv");
							continue ;
						}
						buf[rc] = '\0';
						const Context &context = (ctx[fd] += std::string(buf));
						if (context.full)
						{
							server->info("data received: " + context.plain);
							server->respond_client(fd, context);
							server->info("closing connection");
							FD_CLR(fd, &master_set);
							close(fd);
							ctx.erase(fd);
						}
					}
					catch (std::exception &e)
					{
						errorpage("500", "Internal Server Error", fd);
						server->info(std::string("error: ") + e.what() + ", closing connection");
					}
				}
			}
		}
		delete [] buf;
		return (NULL);
	}
};
