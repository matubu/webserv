/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strtolower.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 02:27:40 by bledda            #+#    #+#             */
/*   Updated: 2022/02/11 02:30:33 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/utils.hpp"

std::string strtolower(std::string str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); it++)
		*it = tolower(*it);
	return (str);
}
