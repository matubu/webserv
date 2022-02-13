/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   itos.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmehran <mmehran@student.42nice.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 07:37:49 by bledda            #+#    #+#             */
/*   Updated: 2022/02/08 15:51:40 by mmehran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/webserv.hpp"
#include <string>
#include <sstream>

std::string itos(int number)
{
	std::ostringstream ss;
	ss << number;
	return ss.str();
}
