/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 20:12:07 by bledda            #+#    #+#             */
/*   Updated: 2022/02/09 10:31:41 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/webserv.hpp"

typedef struct sockaddr t_sockaddr;
typedef struct sockaddr_in t_sockaddr_in;
typedef struct in_addr t_in_addr;

t_sockaddr_in setup_server(int port, int * server_socket)
{
	*server_socket = socket(AF_INET, SOCK_STREAM, 6);
	t_sockaddr_in	server_interface = {0};
	int opt = 1;

	signal(SIGINT, handler);
	server_interface.sin_addr.s_addr = htonl(INADDR_ANY);
	server_interface.sin_family = AF_INET;
	server_interface.sin_port = htons(port);
	if(*server_socket == -1)
	{
		webserv::perror("socket()");
		handler(SIGINT);
		exit(errno);
	}
	// Forcefully attach socket to the port
	if (setsockopt(*server_socket, SOL_SOCKET, SO_REUSEADDR,
												&opt, sizeof(opt)))
	{
		webserv::perror("setsockopt()");
		handler(SIGINT);
		exit(EXIT_FAILURE);
	}
	while(bind(*server_socket,
		(t_sockaddr *) &server_interface,
		sizeof(server_interface)) == -1)
	{
		webserv::perror("bind()");
		webserv::log("Next try in 3 sec");
		if (errno != 48)
		{
			webserv::perror("bind()");
			handler(SIGINT);
			exit(errno);
		}
		sleep(3);
	}
	if(listen(*server_socket, 5) == -1)
	{
		webserv::perror("listen()");
		handler(SIGINT);
		exit(errno);
	}
	return (server_interface);
}

void printInfo(std::map<std::string, std::string> info)
{
	std::map<std::string, std::string>::iterator it = info.begin();

	for (; it != info.end(); it++)
	{
		std::string val = "[";
		val += it->first;
		val += "] ";
		val += it->second;
		webserv::debug(val);
	}
}

std::map<std::string, std::string> getInfo(std::string data)
{
	std::map<std::string, std::string> value;
	std::string line;
	std::string	key;
	int j, i;

	for (i = 0; data[i]; i++)
	{
		while (data[i] && data[i] != '\n')
			line += data[i++];
		for (j = 0; j < line.size() && line[j] != ' ' && line[j] != ':'; j++)
		{
			key += line[j];
		}
		if (line[j] == ' ')
			j++;
		else if (line[j] == ':')
			j += 2;
		value[key] = line.substr(j, line.size());
		line.clear();
		key.clear();
	}
	return (value);
}

std::string getBufferHeader(int bufferSize, int client_socket)
{
	char buffer[bufferSize];
	int n = 0;
	if ((n = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) < 0)
	{
		webserv::perror("recv()");
		handler(SIGINT);
		exit(errno);
	}
	buffer[n] = 0;

	return (buffer);
}

void gestion_client(t_sockaddr_in *server_interface, int server_socket)
{
	t_sockaddr_in client_interface = {0};
	socklen_t size_client_interface = sizeof(client_interface);

	while (1)
	{
		int client_socket = accept(server_socket,
			(t_sockaddr *)server_interface, &size_client_interface);
	
		if(client_socket == -1)
		{
			webserv::perror("accept()");
			handler(SIGINT);
			exit(errno);
		}
		
		std::string buffer = getBufferHeader(1024, client_socket);

		std::map<std::string, std::string> value = getInfo(buffer);
		// printInfo(value);

		std::string send;

		send = "HTTP/1.1 200 OK\r\n";
		
		std::string HTTP_ACCEPT = "HTTP_ACCEPT=";
		HTTP_ACCEPT += value["Accept"];

		std::string HTTP_USER_AGENT = "HTTP_USER_AGENT=";
		HTTP_USER_AGENT += value["User-Agent"];

		std::string HTTP_ACCEPT_LANGUAGE= "HTTP_ACCEPT_LANGUAGE=";
		HTTP_ACCEPT_LANGUAGE += value["Accept-Language"];

		char **env = static_cast<char **>(calloc(sizeof(char *), 50));

		env[0] = strdup("REQUEST_METHOD=GET");
		env[1] = strdup("SERVER_PROTOCOL=HTTP/1.1");
		env[2] = strdup("PATH_INFO=/");
		env[3] = strdup(HTTP_ACCEPT.c_str());
		env[4] = strdup(HTTP_USER_AGENT.c_str());
		env[5] = strdup("SCRIPT_FILENAME=/Users/bledda/Documents/webserv/phpinfo.php");
		
		int pipefd[2];
		pipe(pipefd);

		if (fork() == 0)
		{
			close(pipefd[0]);

			dup2(pipefd[1], 1);
			dup2(pipefd[1], 2);

			close(pipefd[1]);

			std::string config_php = "./php.ini";
			if (execve("./php-cgi", (config_php.c_str(), nullptr), env) == -1)
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
					send = "HTTP/1.1 ";
					send += line.substr(8, 7);
				}
				if (line.empty())
					continue ;
				webserv::log(line);
				send += line;
				line.clear();
			}
		}

		if(sendto(client_socket, send.c_str(), send.size(), 0,
			(t_sockaddr *)&client_socket, size_client_interface) < 0)
		{
			webserv::perror("sendto()");
			handler(SIGINT);
			exit(errno);
		}

		close(client_socket);
	}
}

int main(void)
{
	t_sockaddr_in server_interface;
	
	webserv::autoindex::readHTML();
	int server_socket;
	server_interface = setup_server(8080, &server_socket);
	webserv::success("Server is ready");
	gestion_client(&server_interface, server_socket);
	return (0);
}
