/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Autoindex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/13 04:14:15 by bledda            #+#    #+#             */
/*   Updated: 2022/02/13 19:16:59 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "utils.hpp"
#include <dirent.h>

struct Autoindex
{
	static int get_file_size(std::string filename) {
		FILE *p_file = NULL;
		int size;

		p_file = fopen(filename.c_str(), "rb");
		fseek(p_file, 0, SEEK_END);
		size = ftell(p_file);
		fclose(p_file);
		return size;
	}

	static std::string setLocation(std::string location) {
		std::string tmp = "<h2>" + location + "</h2></div>";
		return (tmp);
	}

	static std::string item(std::string url, std::string name,
							std::string date, std::string octe)
	{
		std::string tmp = "<div class=\"files\"><a href=\"" + url + "\">"
				+ name + "</a><p>" + date + "</p><p>" + octe + " Ko </p></div>";
		return (tmp);
	}

	static std::string top() {
		std::string top = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>WebServ Directory</title><link href=\"https://fonts.googleapis.com/css2?family=Montserrat&display=swap\"rel=\"stylesheet\"><style>* {margin: 10px;padding: 0;font-family: 'Montserrat', sans-serif;color:white;background-color: black;}img {display:block;margin:auto;width: 150px;background-color:transparent;}h1, h2, h4, h5, h6, p, a {background-color:transparent;}h4 {font-size: 16px;}h5 {font-size: 14px;}h6 {font-size: 12px;}body {margin:auto;max-width: 720px;}.hero{display: block;margin: auto;background-color: rgb(16, 181, 187);border-radius: 10px;padding: 50px;padding-bottom: 10px;color: white;}.hero h1 {padding-top: 23px;}.files {display: flex;flex-direction: row;flex-wrap: nowrap;justify-content: center;align-items: center;background-color: rgb(12, 142, 146);border-radius: 8px 1px 10px 5px;text-align: center;margin-left: 2;margin-right: 2;}.files a, .files p {flex-basis:  33%;}.files img {display:block;height: 20px;width: 20px;border:1px solid black}.files a {color:white;}.footer {background-color: rgb(16, 181, 187);width: 100%;text-align: center;border-radius: 10px;display: block;margin: auto;padding-top: 5px;padding-bottom: 10px;}.footer h6 a {color: white};</style></head><body><div class=\"hero\"><img	src=\"https://www.42nice.fr/static/images/logo-white.png\"alt=\"42 Nice logo\"/><h1>WebServ index of</h1>";
		return (top);
	}
	static std::string end() {
		std::string end = "<div class=\"footer\"><h4>WebServer - 2022</h4><h5>42 Nice</h5><h6><a href=\"https://github.com/matubu\" target=\"_blank\" alt=\"github mberger-\">mberger-</a> ,<a href=\"https://github.com/mathieupi\" target=\"_blank\" alt=\"github mmehran-\">mmheran</a> ,<a href=\"https://github.com/louchebem06\" target=\"_blank\" alt=\"github bledda\">bledda</a></h6></div></body></html>";
		return (end);
	}
	
	static std::string page(std::string path)
	{
		std::string 	autoindex = Autoindex::top();
		DIR 			*dir;
		struct dirent	*ent;
		
		if ((dir = opendir(path.c_str())) != NULL) {
			autoindex += Autoindex::setLocation(path);
			while ((ent = readdir(dir)) != NULL)
			{
				if (ent->d_name[0] == '.' && ent->d_name[1] == 0)
					continue ;
				std::string tmp = path + "/";
				tmp += ent->d_name;
				struct stat t_stat;
				stat(tmp.c_str(), &t_stat);
				struct tm * timeinfo = localtime(&t_stat.st_ctime);
				autoindex += Autoindex::item(tmp,
							ent->d_name, asctime(timeinfo),
							atos(Autoindex::get_file_size(tmp)));
			}
			closedir (dir);
		}
		else {
			perror ("opendir()");
			exit(errno);
		}
		autoindex += Autoindex::end();
		return (autoindex);
	}
};
