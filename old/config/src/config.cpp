/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 02:35:03 by bledda            #+#    #+#             */
/*   Updated: 2022/02/11 12:06:15 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/config.hpp"

static void open_file(std::ifstream & ifs, std::string file)
{
	struct stat		fileInfo;

	if (!stat(file.c_str(), &fileInfo) && fileInfo.st_mode & S_IFDIR)
	{
		webserv::error(file + " is not a file");
		exit (1);
	}
  	ifs.open(file.c_str());
	if (!ifs.is_open())
	{
		webserv::error("An error occurred while opening the file");
		exit (1);
	}
}

static void in_server(std::ifstream & ifs, std::string line,
					bool & scope_is_open, bool & is_server, int n_line)
{
	if (line == "server" && !scope_is_open)
		is_server = true;
	else if (line == "{" && is_server && !scope_is_open)
	{
		scope_is_open = true;
		is_server = false;
	}
	else if (line == "{" && scope_is_open)
	{
		ifs.close();
		webserv::error("A scope should have been closed before\
						\n\t\tline (" + itos(n_line) + ") : \"" + line + "\"");
		exit (1);
	}
	else if (line == "}" && scope_is_open)
		scope_is_open = false;
	else if (!scope_is_open)
	{
		ifs.close();
		webserv::error("The configuration must be in scopes \
						\n\t\tline (" + itos(n_line) + ") : \"" + line + "\"");
		exit (1);
	}
}

static bool	normalize_line(std::ifstream & ifs, std::string & line, size_t & n_line)
{
	std::string::iterator it = line.begin();

	n_line++;
	while (isspace(*it) && !isalnum(*it) && it++ != line.end()) ;
	line.erase(line.begin(), it);
	if (line.substr(0,2) == "//" || line.empty())
		return (true);
	else if (line.find_first_of("{}") != std::string::npos && line.size() != 1)
	{
		ifs.close();
		webserv::error("The configuration file is invalid \
						\n\t\tline (" + itos(n_line) + ") : \"" + line + "\"");
		exit (1);
	}
	return (false);
}

static void print_config(t_config config)
{
	typedef std::string	string;

	webserv::debug("PORT : " + config.port);
	webserv::debug("HOST : " + config.host);
	webserv::debug("BODY SIZE : " + config.body_size);
	
	webserv::debug("SERVER NAME : ");
	std::vector<string>::iterator it_vec = config.server_name.begin();
	for (; it_vec != config.server_name.end(); it_vec++)
		webserv::debug("\tVALUE : " + *it_vec);
	webserv::debug("ERROR : ");
	std::map<string, string>::iterator it_error = config.error.begin();
	for (; it_error != config.error.end(); it_error++)
	{
		webserv::debug("\tCODE : " + (*it_error).first);
		webserv::debug("\tREDIRECT : " + (*it_error).second);
	}
	std::vector<string>::iterator it = config.location.begin();
	for (; it != config.location.end(); it++)
	{
		webserv::debug("URL RULE : " + *it);
		webserv::debug("\tAUTOINDEX : "
					+ std::string((config.autoindex[*it]) ? "True" : "False"));
		webserv::debug("\tROOT : " + config.root[*it]);
		webserv::debug("\tINDEX FILE : " + config.index_file[*it]);
		webserv::debug("\tMETHOD ACCEPT : ");
		std::vector<string> tmp_vector = config.method[*it];
		std::vector<string>::iterator it_method = tmp_vector.begin();
		for (; it_method != tmp_vector.end(); it_method++)
			webserv::debug("\t\tACCEPT : " + *it_method);
		webserv::debug("\tCGI :");
		std::map<string, string> tmp_map = config.cgi[*it];
		std::map<string, string>::iterator it_cgi = tmp_map.begin();
		for (; it_cgi != tmp_map.end(); it_cgi++)
		{
			webserv::debug("\t\tEXTENSION FILES : " + (*it_cgi).first);
			webserv::debug("\t\tCGI USE : " + (*it_cgi).second);
		}
		webserv::debug("\tREDIRECTION :");
		std::pair<string, string> tmp_pair = config.redirect[*it];
		webserv::debug("\t\tCODE REDIRECT : " + tmp_pair.first);
		webserv::debug("\t\tURL REDIRECT : " + tmp_pair.second);
	}
}

static bool isUrlKey(std::string key)
{
	if (key.size() <= 5)
		return (false);
	if ((key.substr(0, 4) == "url<" && *(key.end()-1) == '>'))
		return (true);
	return (false);
}

static std::string getLocation(std::string key)
{
	return (key.substr(4, key.size() - 5));
}

static void validKey(std::ifstream & ifs, std::string key)
{
	if (key != "host"
		&& key != "port"
		&& key != "server_name"
		&& key != "body_size"
		&& key != "error"
		&& !isUrlKey(key))
	{
		ifs.close();
		webserv::error("\"" + key + "\" in key, is not valid");
		exit (1);
	}
}

static std::string normalize_str(std::string str)
{
	str.erase(remove_if(str.begin(), str.end(), isspace), str.end());
	return (str);
}

static bool exist(std::vector<std::string> test, std::string search)
{
	std::vector<std::string>::iterator it = test.begin();

	for (; it != test.end(); it++)
		if (*it == search)
			return (true);
	return (false);
}

static void add_param(std::ifstream & ifs, t_config & config,
						std::string & params, std::string key)
{
	std::vector<std::string>			value;
	std::vector<std::string>			tmp;
	std::vector<std::string>::iterator	it;

	value = split(params, ",");
	for (it = value.begin(); it != value.end(); it++)
	{
		if (!isUrlKey(key))
		{
			if (key == "port")
			{
				if (!strisdigit(normalize_str(*it)))
				{
					ifs.close();
					webserv::error("Port must and a number");
					exit (1);
				}
				else if (normalize_str(*it).empty())
				{
					ifs.close();
					webserv::error("Port Can not be empty");
					exit (1);
				}
				else if (config.port.empty())
					config.port = normalize_str(*it);
				else
				{
					ifs.close();
					webserv::error("Port already exists \"" + config.port + "\" redefinition in \"" + normalize_str(*it) + "\" not possible");
					exit (1);
				}
			}
			else if (key == "host")
			{
				if (normalize_str(*it).empty())
				{
					ifs.close();
					webserv::error("Host Can not be empty");
					exit (1);
				}
				else if (config.host.empty())
					config.host = normalize_str(*it);
				else
				{
					ifs.close();
					webserv::error("Host already exists \"" + config.host + "\" redefinition in \"" + normalize_str(*it) + "\" not possible");
					exit (1);
				}
			}
			else if (key == "body_size")
			{
				if (!strisdigit(normalize_str(*it)))
				{
					ifs.close();
					webserv::error("Body_size must and a number");
					exit (1);
				}
				else if (config.body_size.empty())
					config.body_size = normalize_str(*it);
				else
				{
					ifs.close();
					webserv::error("Body_size already exists \"" + config.body_size + "\" redefinition in \"" + normalize_str(*it) + "\" not possible");
					exit (1);
				}
			}
			else if (key == "error")
			{
				if (value.size() < 2)
				{
					ifs.close();
					webserv::error("Error not enough arguments");
					exit (1);
				}
				if (it == value.end() - 1)
					continue ;
				if (!strisdigit(normalize_str(*it)))
				{
					ifs.close();
					webserv::error("Error must and a number");
					exit (1);
				}
				else if (config.error.count(normalize_str(*it)))
				{
					ifs.close();
					webserv::error("Error already exists in \"" + normalize_str(*it) + "\" redefinition in \"" + normalize_str(*(value.end()-1)) + "\" not possible");
					exit (1);
				}
				else
					config.error[normalize_str(*it)] = normalize_str(*(value.end()-1));
			}
			else if (key == "server_name")
			{
				if (value.size() < 2)
				{
					ifs.close();
					webserv::error("Server_name not enough arguments");
					exit (1);
				}
				else if (normalize_str(*it).empty())
				{
					ifs.close();
					webserv::error("Server_name empty value is not possible");
					exit (1);
				}
				else if (exist(config.server_name, normalize_str(*it)))
				{
					ifs.close();
					webserv::error("Server_name \"" + normalize_str(*it) + "\" already exists");
					exit (1);
				}
				config.server_name.push_back(normalize_str(*it));
			}
		}
		else
		{
			if (!exist(config.location, getLocation(key)))
				config.location.push_back(getLocation(key));
			tmp = split(*it, " ");
			for (size_t i = 1; i < tmp.size(); i++)
			{
				if (tmp[0] == "autoindex")
				{
					if (strtolower(tmp[i]) == "true" || tmp[i] == "1")
						config.autoindex[getLocation(key)] = true;
					else if (strtolower(tmp[i]) == "false" || tmp[i] == "0")
						config.autoindex[getLocation(key)] = false;
					else
					{
						ifs.close();
						webserv::error("Value incorect autoindex in \"" + key + "\"");
						exit (1);
					}
				}
				else if (tmp[0] == "root")
				{
					if (tmp.size() != 2)
					{
						ifs.close();
						webserv::error("Incorect value root in \"" + key + "\"");
						exit (1);
					}
					config.root[getLocation(key)] = tmp[i];
				}
				else if (tmp[0] == "index_file")
				{
					if (tmp.size() != 2)
					{
						ifs.close();
						webserv::error("Incorect value index_file in \"" + key + "\"");
						exit (1);
					}
					config.index_file[getLocation(key)] = tmp[i];
				}
				else if (tmp[0] == "method")
				{
					if (tmp.size() < 2)
					{
						ifs.close();
						webserv::error("Incorect value method in \"" + key + "\"");
						exit (1);
					}
					config.method[getLocation(key)].push_back(tmp[i]);
				}
				else if (tmp[0] == "redirect")
				{
					if (tmp.size() != 3)
					{
						ifs.close();
						webserv::error("Incorect value redirect in \"" + key + "\"");
						exit (1);
					}
					config.redirect[getLocation(key)] = make_pair(tmp[1], tmp[2]);
				}
				else if (tmp[0] == "cgi")
				{
					if (tmp.size() != 3)
					{
						ifs.close();
						webserv::error("Incorect value cgi in \"" + key + "\"");
						exit (1);
					}
					config.cgi[getLocation(key)][tmp[1]] = tmp[2];
				}
				else
				{
					ifs.close();
					webserv::error("Rules \"" + tmp[0] + "\" in URL is invalide");
					exit (1);
				}
			}
		}
	}
}

static t_config add_config(std::ifstream & ifs, std::string conf)
{
	t_config	config;
	bool		find_key = false;
	std::string tmp;
	std::string key;

	std::string::iterator it;
	for (it = conf.begin(); it != conf.end(); it++)
	{
		if (*it == ':' && find_key == false)
		{
			validKey(ifs, tmp);
			find_key = true;
			key = tmp;
			tmp.clear();
		}
		else if (*it == ';' && find_key == true)
		{
			add_param(ifs, config, tmp, key);
			find_key = false;
			tmp.clear();
		}
		else
			tmp += *it;
	}

	if (find_key)
	{
		ifs.close();
		webserv::error("A parameter of one of the servers does not have a ';'");
		exit (1);
	}

	return (config);
}

std::vector<t_config> config_file(std::string file)
{
	std::ifstream			ifs;
	std::string				line;
	size_t 					n_line = 0;
	bool 					is_server = false;
	bool 					scope_is_open = false;
	std::string				one_line_config;
	std::vector<t_config>	config;

	open_file(ifs, file);
	while (std::getline(ifs, line))
	{
		if (normalize_line(ifs, line, n_line))
			continue ;
		in_server(ifs, line, scope_is_open, is_server, n_line);

		if (line == "server" || line == "{")
			continue ;
		else if (line == "}" && !one_line_config.empty())
		{
			config.push_back(add_config(ifs, one_line_config));
			one_line_config.clear();
		}
		else
			one_line_config += line;
	}

	ifs.close();

	if (scope_is_open)
	{
		webserv::error("A scope is missing at the end of the file");
		exit (1);
	}

	std::vector<t_config>::iterator it;
	for (it = config.begin(); it != config.end(); it++)
	{
		print_config(*it);
		std::cout << std::endl;
	}

	webserv::success("Configuration load successfully");
	return (config);
}
