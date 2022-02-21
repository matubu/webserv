#pragma once

#include "Request.hpp"
#include <iostream>
#include <string>
#include "utils.hpp"
#include <vector>
#include "Route.hpp"

class Url
{
	public:
		std::string path_info;
		std::string root;
		std::string absolute;
	public:
		Url(std::string original_url, std::map<std::string, Route> &routes)
		{
			root = getRoot(original_url, routes);
			if (root == original_url)
				return ;
			original_url = original_url.substr(root.size());
			if (routes[root].cgi.size() != 0)
			{
				bool found = false;
				std::map<std::string, std::string>::iterator it = routes[root].cgi.begin();
				for (size_t i = 0; i < routes[root].cgi.size(); i++)
				{
					std::string ext = (*it).first;
					size_t pos = original_url.find(ext);
					if (pos != std::string::npos)
					{
						absolute += original_url.substr(0, pos + ext.size());
						path_info = original_url.substr(pos + ext.size());
						found = true;
						break ;
					}
					it++;
				}
				if (!found)
					absolute = ((original_url[original_url.size() - 1] == '/')
							? original_url.substr(0, original_url.size() - 1) : original_url);
			}
			else
				absolute = original_url;
			std::cout << "final path : " << absolute << std::endl;
			std::cout << "path_info : " << path_info << std::endl;
		};
		~Url() {};
	private:
		std::string getRoot(std::string _url, std::map<std::string, Route> &routes)
		{
			std::vector<std::string> split_url = split(_url, "/");
			size_t i = split_url.size();

			while (i >= 0)
			{
				std::string tmp = tryRoot(split_url, i);
				std::cout << "find root : " << tmp << std::endl;
				if (routes.count((tmp == "/") ? tmp : tmp + "/"))
					return ((tmp == "/") ? tmp : tmp + "/");
				i--;
			}
			return "";
		}
		std::string tryRoot(std::vector<std::string> _split, size_t i)
		{
			std::string tmp;
			if (i == 0)
				return ("/");
			for (size_t j = 0; j < i; j++)
				tmp += "/" + _split[j];
			return (tmp);
		}
};
