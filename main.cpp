/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 20:12:07 by bledda            #+#    #+#             */
/*   Updated: 2022/02/07 22:36:18 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <map>
#include <vector>
#include "color.h"

typedef struct sockaddr t_sockaddr;
typedef struct sockaddr_in t_sockaddr_in;
typedef struct in_addr t_in_addr;

int g_server_socket = -1;

void  handler(int sig)
{
	if (sig != SIGINT)
		return ;
	if (close(g_server_socket) == -1)
	{
		perror("close()");
		exit(errno);
	}
	std::cout << CYAN << "\r[WebServer] Server close" << RESET << std::endl;
	exit(1);
}

t_sockaddr_in setup_server(int port)
{
	g_server_socket = socket(AF_INET, SOCK_STREAM, 6);
	t_sockaddr_in	server_interface = {0};

	signal(SIGINT, handler);
	server_interface.sin_addr.s_addr = htonl(INADDR_ANY);
	server_interface.sin_family = AF_INET;
	server_interface.sin_port = htons(port);
	if(g_server_socket == -1)
	{
		perror("socket()");
		handler(SIGINT);
		exit(errno);
	}
	if(bind(g_server_socket,
		(t_sockaddr *) &server_interface,
		sizeof(server_interface)) == -1)
	{
		perror("bind()");
		handler(SIGINT);
		exit(errno);
	}
	if(listen(g_server_socket, 5) == -1)
	{
		perror("listen()");
		handler(SIGINT);
		exit(errno);
	}
	return (server_interface);
}

void printInfo(std::map<std::string, std::vector<std::string> > info)
{
	std::map<std::string, std::vector<std::string> >::iterator it = info.begin();
	std::vector<std::string> tmp;
	std::vector<std::string>::iterator it_vec;

	for (; it != info.end(); it++)
	{
		std::cout << "[" << (*it).first << "] " << std::endl;
		tmp = (*it).second;
		for (it_vec = tmp.begin(); it_vec != tmp.end(); it_vec++)
			std::cout << "\t- " << *it_vec << std::endl;
	}
}

std::map<std::string, std::vector<std::string> > getInfo(char *data)
{
	std::map<std::string, std::vector<std::string> > value;
	std::string line;
	std::string	key;
	std::vector<std::string> items;
	std::string tmp;
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
		line = line.substr(j, line.size());
		for (j = 0; j < line.size(); j++)
		{
			while (line[j] && line[j] != ',')
				tmp += line[j++];
			j += 2;
			items.push_back(tmp);
			tmp.clear();
		}
		value[key] = items;
		line.clear();
		key.clear();
		items.clear();
	}
	return (value);
}

void gestion_client(t_sockaddr_in *server_interface)
{
	t_sockaddr_in client_interface = {0};
	socklen_t size_client_interface = sizeof(client_interface);

	while (1)
	{
		int client_socket = accept(g_server_socket,
			(t_sockaddr *)server_interface, &size_client_interface);
	
		if(client_socket == -1)
		{
			perror("accept()");
			handler(SIGINT);
			exit(errno);
		}
		char buffer[1024];
		int n = 0;
		if ((n = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) < 0)
		{
			perror("recv()");
			handler(SIGINT);
    		exit(errno);
		}
		buffer[n] = 0;

		std::map<std::string, std::vector<std::string> > value = getInfo(buffer);
		printInfo(value);

		std::string send;

		send = "HTTP/1.1 200 OK\n";
		send += "Server: WebServer\n";
		
		if(sendto(client_socket, send.c_str(), send.size(), 0,
			(t_sockaddr *)&client_socket, size_client_interface) < 0)
		{
			perror("sendto()");
			handler(SIGINT);
			exit(errno);
		}

		close(client_socket);
	}
}

int main(void)
{
	t_sockaddr_in server_interface;
	
	server_interface = setup_server(8080);
	std::cout << GREEN << "[WebServer] Server is ready" << RESET << std::endl;
	gestion_client(&server_interface);
	return (0);
}
