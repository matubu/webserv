/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 02:27:58 by bledda            #+#    #+#             */
/*   Updated: 2022/02/11 02:30:26 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/utils.hpp"

std::vector<std::string> split(std::string s, std::string delimiter)
{
    size_t						pos_start = 0;
	size_t						pos_end;
	size_t						delim_len = delimiter.size();
    std::string 				value;
	std::vector<std::string>	res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
	{
        value = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(value);
    }
    res.push_back(s.substr(pos_start));
    return res;
}
