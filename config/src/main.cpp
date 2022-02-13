/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/10 11:22:10 by bledda            #+#    #+#             */
/*   Updated: 2022/02/11 11:30:54 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/config.hpp"

int main(int ac, char **av)
{
	std::vector<t_config> config;

	if (ac > 2)
		webserv::log("Additional arguments will be ignored");
	if (ac == 2)
		config = config_file(av[1]);
	else
		webserv::log("Using default config");

	return (0);
}
