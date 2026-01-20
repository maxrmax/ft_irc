/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:11:15 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/09 17:48:18 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


#include <algorithm>
#include <iostream>
#include <string>

#include "irc_packet.hpp"
#include "poll.hpp"
#include "server.hpp"

//isDigit.cpp
bool is_digits_only(const std::string &string);

//main.cpp
int check_arguments(int argc, char **argv);

//runServer.cpp
int acceptClient(Server &irc_server, std::vector<pollfd> &poll_fd);
int clients_waiting(Server &irc_server, std::vector<pollfd> &poll_fd);
int process_ready_fd(Server &irc_server, std::vector<pollfd> &poll_fd, int i);
int receive_message(std::vector<pollfd> &poll_fd, int i);
int runPoll(Server &irc_server, std::vector<pollfd> &poll_fd);
int runServer(Server &irc_server);
