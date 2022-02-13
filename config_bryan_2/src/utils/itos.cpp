/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   itos.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 02:26:25 by bledda            #+#    #+#             */
/*   Updated: 2022/02/11 02:30:36 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/utils.hpp"

std::string itos(int number)
{
	std::ostringstream ss;
	ss << number;
	return ss.str();
}
