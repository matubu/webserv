/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv_namespace.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 07:59:46 by bledda            #+#    #+#             */
/*   Updated: 2022/02/09 11:11:35 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/webserv.hpp"

namespace webserv
{
	void perror(const char * val)
	{
		std::cerr << RED << "[WebServ] ";
		std::perror(val);
		std::cerr << RESET;
	}

	void error(std::string val)
	{
		std::cerr << RED << "[WebServ] ";
		std::cerr << val << std::endl;
		std::cerr << RESET;
	}

	void log(std::string val)
	{
		std::cerr << BLUE << "[WebServ] ";
		std::cerr << val << std::endl;
		std::cerr << RESET;
	}

	void success(std::string val)
	{
		std::cerr << GREEN << "[WebServ] ";
		std::cerr << val << std::endl;
		std::cerr << RESET;
	}

	void debug(std::string val)
	{
		std::cerr << YELLOW << "[WebServ] ";
		std::cerr << val << std::endl;
		std::cerr << RESET;
	}

	namespace autoindex
	{
		std::string top;
		std::string end;

		std::string setLocation(std::string location)
		{
			std::string tmp;

			tmp = "<h2>";
			tmp += location;
			tmp += "</h2></div>";
			return (tmp);
		}

		std::string HTML_file(std::string url, std::string name,
								std::string date, std::string octe)
		{
			std::string tmp;
			
			tmp = "<div class=\"files\">";
			tmp += "<a href=\"";
			tmp += url;
			tmp += "\">";
			tmp += name;
			tmp += "</a>";
			tmp += "<p>";
			tmp += date;
			tmp += "</p>";
			tmp += "<p>";
			tmp += octe;
			tmp += " Ko</p>";
			tmp += "</div>";
			return (tmp);
		}

		void readHTML()
		{
			std::ifstream top_file("autoindex/top");
			std::ifstream end_file("autoindex/end");
			
			if (top_file.is_open()) {
				std::string line;
				while (std::getline(top_file, line)) {
					webserv::autoindex::top += line;
				}
				top_file.close();
			}

			if (end_file.is_open()) {
				std::string line;
				while (std::getline(end_file, line)) {
					webserv::autoindex::end += line;
				}
				end_file.close();
			}
		}

		std::string autoindexHTML(std::string path)
		{
			std::string autoindex;
			autoindex = webserv::autoindex::top;

			DIR *dir;
			struct dirent *ent;
			if ((dir = opendir (path.c_str())) != NULL)
			{
				autoindex += webserv::autoindex::setLocation(path);
				while ((ent = readdir (dir)) != NULL)
				{
					if (ent->d_name[0] == '.' && ent->d_name[1] == 0)
						continue ;
					std::string tmp = path;
					tmp += ent->d_name;
					struct stat t_stat;
					stat(tmp.c_str(), &t_stat);
					struct tm * timeinfo = localtime(&t_stat.st_ctime);
					autoindex += webserv::autoindex::HTML_file(tmp,
								ent->d_name, asctime(timeinfo),
								itos(get_file_size(tmp)));
				}
				closedir (dir);
			}
			else
			{
				perror ("opendir()");
				handler(SIGINT);
				exit(errno);
			}

			autoindex += webserv::autoindex::end;
			return (autoindex);
		}
	}
}
