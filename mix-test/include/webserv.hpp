/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 07:33:59 by bledda            #+#    #+#             */
/*   Updated: 2022/02/08 11:23:02 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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
#include "color.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <ctime>
#include <fstream>

std::string itos(int number);
int get_file_size(std::string filename);
void  handler(int sig);

namespace webserv
{
	void perror(const char * val);
	void error(std::string val);
	void log(std::string val);
	void success(std::string val);
	void debug(std::string val);
	namespace autoindex
	{
		void readHTML();
		std::string autoindexHTML(std::string path);
	}
}
