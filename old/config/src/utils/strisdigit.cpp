/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strisdigit.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 02:27:15 by bledda            #+#    #+#             */
/*   Updated: 2022/02/11 02:30:30 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/utils.hpp"

bool strisdigit(std::string str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); it++)
		if (!isdigit(*it))
			return (false);
	return (true);
}
