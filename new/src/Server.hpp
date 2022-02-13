#pragma once

#include "Route.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "mime.hpp"
#include "utils.hpp"

#define E(code) { \
	server->info("E" # code); \
	if (send(new_sock, # code, 3, 0) == -1) \
		server->perr("send " # code); \
	close(new_sock); \
	continue ; \
}

std::string headers(const std::string &code, size_t len, const std::string &type)
{
	return ("HTTP/1.1 " + code + "\r\nContent-length: " + atos(len) + "\r\nContent-Type: " + type + "\r\n");
}

void sendf(int new_sock, const std::string &path, struct stat &info)
{
	std::string header = headers("200 OK", info.st_size, mime(path)) + "\r\n";
	send(new_sock, header.c_str(), header.size(), 0);
	int fd = open(path.c_str(), O_RDONLY);
	#ifdef __APPLE__
		struct sf_hdtr	hdtr = { NULL, 0, NULL, 0 };
		off_t len = 0;
		sendfile(new_sock, fd, 0, &len, &hdtr, 0);
	#else
		long int off = 0;
		while (sendfile(new_sock, fd, &off, SENDFILE_BUF))
			;
	#endif
	close(fd);
}

class	Server {
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
			std::cout <<url << " " << save << ENDL;
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
		return (Response(404, error.count(404) && exist(error[404], &info) ? error[404] : DEFAULT_404_FILE, NULL));
	}

	void	closeSocket()
	{
		info("closing socket");
		close(sock);
		info("socket closed");
	}

	#define SERVER_ERROR(msg) { \
		server->perr(msg); \
		return (NULL); \
	} 

	/*** START ***/
	static void	*start(Server *server)
	{
		server->info("starting ...");

		int					new_sock;
		struct sockaddr_in	addr;
		socklen_t			addrlen = sizeof(addr);
		int	opt = 1;

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = server->host;
		addr.sin_port = htons(server->port);

		/*** SETUP ***/
		if ((server->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			SERVER_ERROR("cannot create socket");
		setsockopt(server->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		if (bind(server->sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
			SERVER_ERROR("cannot bind");
		if (listen(server->sock, 10) < 0)
			SERVER_ERROR("cannot listen");

		server->info("started");

		while (1) {
			try {
				/*** ACCEPT ***/
				if ((new_sock = accept(server->sock, (struct sockaddr *) &addr, &addrlen)) < 0)
				{
					server->perr(ENDL "cannot accept client");
					continue ;
				}

				server->info("parsing request");

				/*** PARSE ***/
				Request req(new_sock, server->body_size);
				std::cout << RED << req.type << GRE " " << req.url << BLU " " << req.protocol << ENDL;

				/*** FINDING ROUTE ***/
				Response res = server->match(req.url);

				server->info(RED "route found " + res.path);

				/*** CGI ***/
				//if (path == cgi)
				//	cgi();
				//else
				//	senf();

				/*** SEND ***/
				struct stat	info;
				if (stat(res.path.c_str(), &info) == -1)
					E(404)
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
							E(403);
						while ((diread = readdir(dir)))
						{
							struct stat	info;
							char		date[128];
							if (!strcmp(diread->d_name, ".")) continue ;
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
						send(new_sock, s.c_str(), s.size(), 0);
					}
					else
						E(403);
				}
				else
					sendf(new_sock, res.path, info);

				/*** CLOSE ***/
				close(new_sock);
			}
			catch (const std::exception &e)
			{
				// E500
				E(500);
				server->perr(e.what());
			}
		}
		return (NULL);
	}
};