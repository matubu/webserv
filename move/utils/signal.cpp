/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 07:58:29 by bledda            #+#    #+#             */
/*   Updated: 2022/02/08 08:08:55 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/webserv.hpp"

void  handler(int sig)
{
	if (sig != SIGINT)
		return ;
	std::cout << "\b\b  \b\b";
	webserv::log("Server close");
	exit(1);
}
