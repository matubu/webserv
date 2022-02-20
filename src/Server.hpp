#pragma once

#include "Route.hpp"
#include "Request.hpp"
#include "utils.hpp"
#include "cgi.hpp"
#include "Response.hpp"

class Server {
	public:
	int								sock;
	int								port;
	in_addr_t						host;
	std::set<std::string>			name;
	std::map<int, std::string>		error;
	size_t							body_size;
	std::map<std::string, Route>	routes;

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

	void	debug() const
	{
		std::cout << ENDL;
		std::cout << "port " << port << ENDL;
		std::cout << "host " << (host & 255) << "." << (host >> 8 & 255) << "." << (host >> 16 & 255) << "." << (host >> 24) << ENDL;
		for (std::set<std::string>::iterator it = name.begin(); it != name.end(); it++)
			std::cout << "name " << *it << ENDL;
		for (std::map<int, std::string>::const_iterator it = error.cbegin(); it != error.cend(); it++)
			std::cout << "error " << it->first << " " << it->second << ENDL;
		std::cout << "body_size " << body_size << ENDL;
		for (std::map<std::string, Route>::const_iterator it = routes.cbegin(); it != routes.cend(); it++)
		{
			std::cout << "match " << it->first << ENDL;
			it->second.debug();
		}
	}

	bool read_client(int fd)
	{
		char	buf[2049];

		int	rc = recv(fd, buf, 2048, 0);
		if (rc == -1)
			return (false);
		buf[rc] = '\0';
		Request &req = ctx[fd];

		try {
			req.addContent(std::string(buf), name);
			if (req.content.raw.size() > body_size)
			{
				req.response.setError(413, error);
				return (true);
			}
		}
		catch (int e)
		{
			if (e)
				req.response.setError(e, error);
			return (true);
		}
		if (req.ended())
		{
			handle_client(req);
			return (true);
		}
		return (false);
	}

	bool	tryroot(Request &req, const Route &route, const std::string &path, std::string path_info = "")
	{
		std::cout << "try root" << std::endl;
		if (!route.method.count(req.type))
		{
			req.response.setError(405, error);
			return (true);
		}
		/*** REDIRECT ***/
		if (route.redirect.first)
		{
			req.response.setRedirect(route.redirect.first, route.redirect.second + "/" + path);
			return (true);
		}

		if (route.root.empty())
			return (false);
		std::string uri = route.root + path;
		struct stat	stats;
		if (exist(uri + "/" + route.index, &stats))
			uri += "/" + route.index;
		else if (!exist(uri, &stats))
			return (false);


		if (req.type == "DELETE")
		{
			std::string header;
			if (remove(uri.c_str()) == 0)
				req.response.setBody("HTTP/1.1 204 No Content\r\n\n", "");
			else
				req.response.setBody("HTTP/1.1 500 Internal Server Error\r\n\n", "");
			return (true);
		}

		/*** CGI ***/
		std::string cgi = findCgi(route.cgi, uri);
		if (!cgi.empty())
		{
			handleCgi(error, req, uri, cgi, path_info);
			return (true);
		}

		/*** SEND ***/
		if (stats.st_mode & S_IFDIR)
		{
			if (route.autoindex)
				req.response.setAutoindex(error, req.url, uri);
			else
				req.response.setError(403, error);
			return (true);
		}
		req.response.setFd(headers(200, stats.st_size, mime(uri)), open(uri.c_str(), O_RDONLY));
		return (true);
	}

	void handle_client(Request &req)
	{
		std::cout << "handle client" << std::endl;
		if (req.url.find("..") != std::string::npos)
			return (req.response.setError(400, error));
		/*** FINDING ROUTE ***/
		std::string url = req.url;
		std::string	path;

		url = replaceAll(url, "+", " ");
		while (1)
		{
			if (routes.count(url))
				break ;
			if (url == "/") return (req.response.setError(404, error));
			size_t idx = url.find_last_of('/', url.size() - 2);
			path = url.substr(idx + 1, url.size() - idx - 1) + path;
			url = url.substr(0, idx + 1);
		}
		
		std::vector<std::string> u = split(path, "/");
		if (u.size() == 0)
		{
			tryroot(req, routes[url], popchar(path));
			return ;
		}
		struct stat buff;
		std::string file;
		file = routes[url].root + *u.begin();
		std::string path_info;
		for (std::vector<std::string>::iterator it = u.begin() + 1; it != u.end(); it++)
		{

			if (stat(file.c_str(), &buff) != -1 && S_ISREG(buff.st_mode) != 0)
			{
				for (std::vector<std::string>::iterator it2 = it; it2 != u.end(); it2++)
					path_info += *it2 + "/";
				
				size_t	find = file.find(routes[url].root);
				if (find != std::string::npos)
					file.replace(find, routes[url].root.size(), "");

				if (!tryroot(req, routes[url], file, path_info))
					req.response.setError(404, error);
				return ;
			}
			file += "/" + *it;
		}
		if (!tryroot(req, routes[url], popchar(path)))
			req.response.setError(404, error);
	}

	void	initsocket(int *sock, int *kq)
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

		if (bind(*sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
			throw "cannot bind";

		if (listen(*sock, MAX_CONNECTIONS) == -1)
			throw "cannot listen";

		if ((*kq = kqueue()) == -1)
			throw "cannot create kqueue";
	}

	/*** START ***/
	static void	*start(Server *server)
	{
		server->info("starting ...");

		int							sock, new_sock, kq;
		std::vector<struct kevent>	change_lst;
		std::vector<struct kevent>	events_lst;

		events_lst.resize(1);

		try { server->initsocket(&sock, &kq); }
		catch (const char *e)
		{
			server->syserr(e);
			return (NULL);
		}

		server->info("sock " + atos(sock));

		change_lst.resize(1);
		EV_SET(change_lst.data(), sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

		server->info("started");

		while (1)
		{
			int	evt;
			server->info("calling kevent");
			if ((evt = kevent(kq,
					change_lst.data(), change_lst.size(), 
					events_lst.data(), events_lst.size(),
					NULL)) == -1)
			{
				server->syserr("kevent failed");
				break ;
			}
			change_lst.clear();
			std::cout << "new events " << evt << "/" << events_lst.size() << std::endl;
			while (evt--)
			{
				if (events_lst[evt].flags & EV_ERROR)
				{
					std::cout << "event error " << events_lst[evt].ident << " " << strerror(events_lst[evt].data) << std::endl;
					continue ;
				}
				if (events_lst[evt].ident == (uintptr_t)sock)
				{
					//for in client_to_accept
					new_sock = accept(events_lst[evt].ident, NULL, NULL);
					if (new_sock == -1)
					{
						server->syserr("cannot accept client");
						continue ;
					}
					fcntl(new_sock, F_SETFL, O_NONBLOCK);
					change_lst.resize(change_lst.size() + 1);
					events_lst.resize(events_lst.size() + 1);
					EV_SET(&*(change_lst.end() - 1), new_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
					server->info("client accepted");
					continue ;
				}
				if (events_lst[evt].filter & EVFILT_READ && !server->ctx[events_lst[evt].ident].ended())
				{
					server->info("can read");
					try {
						if (!server->read_client(events_lst[evt].ident))
							continue ;
					}
					catch (std::exception &e)
					{
						server->ctx[events_lst[evt].ident].response.setError(500, server->error);
					}
					change_lst.resize(change_lst.size() + 1);
					EV_SET(&*(change_lst.end() - 1), events_lst[evt].ident, EVFILT_WRITE, EV_ADD, 0, 0, 0);
					continue ;
				}
				if (events_lst[evt].filter & EVFILT_WRITE)
				{
					server->info("can write");
					server->ctx[events_lst[evt].ident].response.write(events_lst[evt].ident);
				}
				if (events_lst[evt].filter & EVFILT_WRITE || events_lst[evt].flags & EV_EOF)
				{
					server->info("closing connection");
					//change list delete with (EV_SET) ?
					close(events_lst[evt].ident);
					server->ctx.erase(events_lst[evt].ident);
					events_lst.erase(events_lst.begin() + evt);
					continue ;
				}
				server->info("nothing ?");
			}
		}
		return (NULL);
	}
};
