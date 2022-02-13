/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_file_size.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 07:42:26 by bledda            #+#    #+#             */
/*   Updated: 2022/02/08 07:43:26 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/webserv.hpp"

int get_file_size(std::string filename)
{
    FILE *p_file = NULL;
	int size;

    p_file = fopen(filename.c_str(), "rb");
    fseek(p_file, 0, SEEK_END);
    size = ftell(p_file);
    fclose(p_file);
    return size;
}
