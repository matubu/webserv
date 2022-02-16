#pragma once

#include "Route.hpp"
#include "Request.hpp"
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
	char							*buf;

	std::map<int, Request>			ctx;

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

	// close_server(all fd to close, ..)
	// {

	// }

	int accept_new_client(int server_sock)
	{
		int	new_sock;

		new_sock = accept(server_sock, NULL, NULL);
		fcntl(new_sock, F_SETFL, O_NONBLOCK);
		return (new_sock);
	}

	bool read_client(int fd)
	{
		int	rc = recv(fd, buf, body_size, 0);
		if (rc == -1)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
				throw std::runtime_error("error recv");
			return false;
		}
		buf[rc] = '\0';
		Request &req = ctx[fd];
		//std::cout << req << std::endl;

		req.addContent(std::string(buf));
		//std::cout << req << std::endl;
		if (req.ended())
		{
			info("handling + closing connection");
			handle_client(fd, req);
			close(fd);
			ctx.erase(fd);
			return true;
		}
		return false;
	}

	//path with start / or empty
	//path without end /
	bool	tryroot(int fd, const Request &req, const Route &route, const std::string &path)
	{
		/*** REDIRECT ***/
		if (route.redirect.first)
		{
			redirect(fd, route.redirect.first, route.redirect.second + "/" + path);
			return (true);
		}

		if (route.root.empty())
			return (false);
		std::string uri = route.root + path;
		struct stat	stats;
		std::cout << "tryuri " << uri << ENDL;
		if (exist(uri + "/" + route.index, &stats))
			uri += "/" + route.index;
		else if (!exist(uri, &stats))
			return (false);

		/*** CGI ***/
		std::string cgi = findCgi(route.cgi, uri);
		if (!cgi.empty())
		{
			handleCgi(fd, req, uri, cgi);
			return (true);
		}

		/*** SEND ***/
		if (stats.st_mode & S_IFDIR)
		{
			if (route.autoindex)
				autoindex(fd, req, uri);
			else
				errorpage("403", "Forbidden", fd);
			return (true);
		}
		sendf(fd, uri, stats);
		return (true);
	}

	//put error if request /../ or other outside root
	void handle_client(int fd, const Request &req)
	{
		/*** FINDING ROUTE ***/
		std::string url = req.url;
		std::string	path;

		std::cout << "GET " << url << ENDL;
		while (1)
		{
			std::cout << url << " " << path << ENDL;
			if (routes.count(url) && tryroot(fd, req, routes[url], popchar(path)))
				return ;
			if (url == "/") return (errorpage("404", "Not Found", fd));
			size_t idx = url.find_last_of('/', url.size() - 2);
			path = url.substr(idx + 1, url.size() - idx - 1) + path;
			url = url.substr(0, idx + 1);
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

		int	on = 1;
		if (setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
			throw "setsockopt failed";

		if (bind(*sock, (struct sockaddr *) &addr, sizeof(addr)) == -1)
			throw "cannot bind";

		if (listen(*sock, MAX_CONNECTIONS) == -1)
			throw "cannot listen";
	}

	/*** START ***/
	static void	*start(Server *server)
	{
		server->info("starting ...");

		int						sock, new_sock;

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

		server->buf = new char[server->body_size + 1];

		while (1) {
			working_set = master_set;

			int	count;
			if ((count = select(max_fd + 1, &working_set, NULL, NULL, NULL)) == -1)
			{
				server->syserr("cannot select");
				break ;
			}
			for (int fd = 0; count && fd <= max_fd; ++fd)
			{
				if (!FD_ISSET(fd, &working_set)) continue ;
				count--;
				if (fd == sock)
				{
					if ((new_sock = server->accept_new_client(sock)) == -1)
					{
						server->syserr(ENDL "cannot accept client");
						continue ;
					}
					server->info(atos(new_sock) + " accepted");
					FD_SET(new_sock, &master_set);
					max_fd = std::max(max_fd, new_sock);
				}
				else
				{
					try {
						if (!server->read_client(fd))
							continue ;
					}
					catch (std::exception &e)
					{
						errorpage("500", "Internal Server Error", fd);
						server->info("error: " + std::string(e.what()) + ", closing connection");
					}
					FD_CLR(fd, &master_set);
					close(fd);
					server->ctx.erase(fd);
				}
			}
		}
		delete [] server->buf;
		return (NULL);
	}
};
