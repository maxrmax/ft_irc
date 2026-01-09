/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:11:15 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/05 10:28:46 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


#include <algorithm>
#include <iostream>
#include <string>

#include "irc_packet.hpp"
#include "server.hpp"

//isDigit.cpp
bool is_digits_only(const std::string &string);

//main.cpp
int check_arguments(int argc, char **argv);

