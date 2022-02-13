/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPage.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/13 04:59:45 by bledda            #+#    #+#             */
/*   Updated: 2022/02/13 18:46:42 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "utils.hpp"

struct ErrorPage
{
	static std::string top(std::string error, std::string error_name) {
		std::string str_error = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><style>@import \"https://fonts.googleapis.com/css?family=Inconsolata\";*{color:white;margin: 10px;padding: 0;font-family: \"Inconsolata\", Helvetica, sans-serif;background-color: black;}h1{font-size: 30px;color: green;}p{font-size: 15px;}.terminal p::before{content: \"WebServ$ \";position: relative;}.terminal p::after{content: \" _\";position: relative;animation: 1s infinite clignote;}@keyframes clignote {0% { opacity: 0; }50% { opacity: 100; }100% { opacity: 0; }}.terminal, .credit{max-width: 800px;display: block;margin: auto;padding-top: 100px;}@keyframes cred {0% { content: \"bledda\"; }50% { content: \"mmheran\" }100% { content: \"mberger-\" }}.credit p::after{content: \"\";position: relative;animation: 0.5s infinite cred;}</style><title>" + error + " - " + error_name + "</title></head><body>";
		return (str_error);
	}
	static std::string end(std::string error, std::string error_name) {
		std::string str_error = "<div class=\"terminal\"><h1>Error " + error + " </h1><p>" + error_name + "</p></div><div class=\"credit\"><p>Create at 42 Nice by: </p></div></body></html>";
		return (str_error);
	}
	static std::string page(std::string error, std::string error_name)
	{
		std::string page = ErrorPage::top(error, error_name);
		page += ErrorPage::end(error, error_name);
		return (page);
	}
};
