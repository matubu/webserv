/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 02:24:19 by bledda            #+#    #+#             */
/*   Updated: 2022/02/11 02:30:22 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/utils.hpp"

namespace webserv
{
	#define NAME_WEBSERV "[WebServ] "

	void perror(const char * val)
	{
		std::cerr << RED << NAME_WEBSERV;
		std::perror(val);
		std::cerr << RESET;
	}

	void error(std::string val)
	{
		std::cerr << RED << NAME_WEBSERV;
		std::cerr << val << std::endl;
		std::cerr << RESET;
	}

	void log(std::string val)
	{
		std::clog << BLUE << NAME_WEBSERV;
		std::clog << val << std::endl;
		std::clog << RESET;
	}

	void success(std::string val)
	{
		std::cout << GREEN << NAME_WEBSERV;
		std::cout << val << std::endl;
		std::cout << RESET;
	}

	void debug(std::string val)
	{
		std::cout << YELLOW << NAME_WEBSERV;
		std::cout << val << std::endl;
		std::cout << RESET;
	}
}
