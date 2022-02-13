/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 02:30:47 by bledda            #+#    #+#             */
/*   Updated: 2022/02/11 10:56:05 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "color.h"
#include <sstream>
#include <cstdio>

namespace webserv
{
	#define NAME_WEBSERV "[WebServ] "

	void perror(const char * val);
	void error(std::string val);
	void log(std::string val);
	void success(std::string val);
	void debug(std::string val);
}

std::vector<std::string>	split(std::string s, std::string delimiter);
bool 						strisdigit(std::string str);
std::string 				strtolower(std::string str);
std::string 				itos(int number);
