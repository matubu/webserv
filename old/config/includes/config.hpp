/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 02:35:05 by bledda            #+#    #+#             */
/*   Updated: 2022/02/11 10:59:36 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "utils.hpp"
#include <map>
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>
#include <algorithm>

typedef struct s_config
{
	typedef std::string	string;

	string						port;
	string						host;
	string						body_size;
	std::map<string, string>	error;
	std::vector<string>			server_name;

	std::vector<string>								location;
	std::map<string, bool>							autoindex;
	std::map<string, string>						root;
	std::map<string, string>						index_file;
	std::map<string, std::vector<string> >			method;
	std::map<string, std::pair<string, string> >	redirect;
	std::map<string, std::map<string, string> >		cgi;
}	t_config;

std::vector<t_config> config_file(std::string file);
