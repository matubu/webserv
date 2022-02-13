/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmehran <mmehran@student.42nice.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 20:12:07 by bledda            #+#    #+#             */
/*   Updated: 2022/02/08 15:43:14 by mmehran          ###   ########.fr       */
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

	server_interface.sin_addr.s_addr = htonl(INADDR_ANY);
	server_interface.sin_family = AF_INET;
	server_interface.sin_port = htons(port);
	if(*server_socket == -1)
	{
		webserv::perror("socket()");
		exit(errno);
	}
	// Forcefully attach socket to the port
	if (setsockopt(*server_socket, SOL_SOCKET, SO_REUSEADDR,
												&opt, sizeof(opt)))
	{
		webserv::perror("setsockopt()");
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
			exit(errno);
		}
		sleep(3);
	}
	if(listen(*server_socket, 5) == -1)
	{
		webserv::perror("listen()");
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
			exit(errno);
		}
		
		std::string buffer = getBufferHeader(1024, client_socket);

		std::cout << buffer << std::endl;

		std::map<std::string, std::string> value = getInfo(buffer);
		// printInfo(value);

		std::string send;

		// std::string URL = ".";
		// for (int i = 0;
		// 	i < value["GET"].size()
		// 	&& value["GET"][i] != '?'
		// 	&& value["GET"][i] != ' '; i++)
		// 	URL += value["GET"][i];

		send = "HTTP/1.1 200 OK\n";
		send += "Server: WebServer\n\n";
		
		// std::ifstream URL_file(URL);
		// if (URL_file.is_open())
		// {
		// 	std::string line;
		// 	while (std::getline(URL_file, line)) {
		// 		send += line;
		// 	}
		// 	URL_file.close();
		// }
		send += webserv::autoindex::autoindexHTML("./");
		
		if(sendto(client_socket, send.c_str(), send.size(), 0,
			(t_sockaddr *)&client_socket, size_client_interface) < 0)
		{
			webserv::perror("sendto()");
			exit(errno);
		}

		close(client_socket);
	}
}

void after_main(void)
{
	std::cout << "\b\b  \b\b";
	webserv::log("Server close");
}

int main(void)
{
	t_sockaddr_in server_interface;
	
	signal(SIGINT, handler);
	atexit(after_main);
	webserv::autoindex::readHTML();
	int server_socket;
	server_interface = setup_server(8080, &server_socket);
	webserv::success("Server is ready");
	gestion_client(&server_interface, server_socket);
	std::cout << "salut" << std::endl;
	return (0);
}
