/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   itos.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 07:37:49 by bledda            #+#    #+#             */
/*   Updated: 2022/02/08 07:41:18 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/webserv.hpp"

std::string itos(int number)
{
	std::string tmp;
    std::string value;
	int i;

    if (number == 0)
        return "0";
    while (number > 0)
    {
        tmp += number % 10 + '0';
        number /= 10;
    }
    for (i = 0; i < tmp.size(); i++)
        value += tmp[tmp.size() - i - 1];
    return value;
}
