/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:11:15 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/05 14:15:41 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>

#include "Client.hpp"
// #include "CmdJoin.hpp"
#include "cmdNick.hpp"
// #include "CmdUser.hpp"
#include "commandDispatcher.hpp"
// #include "inputBuffer.hpp"
#include "irc_packet.hpp"
// #include "OutputBuffer.hpp"
#include "poll.hpp"
#include "server.hpp"

//IRC standard protocol max message length 512
 static const size_t IRC_MAX_LINE = 510; // excluding \r\n

//isDigit.cpp
bool    is_digits_only(const std::string &string);

//main.cpp
int     check_arguments(int argc, char **argv);
void    printCommand(const ParsedCommand& cmd);

//runServer.cpp
int     acceptClient(Server &irc_server, std::vector<pollfd> &poll_fd, std::unordered_map<int, Client> &poll_client__mapping_via_fd);
int     clients_waiting(Server &irc_server, std::vector<pollfd> &poll_fd, std::unordered_map<int, Client> &poll_client__mapping_via_fd);
int     process_ready_fd(Server &irc_server, std::vector<pollfd> &poll_fd, int i);
int     receive_message(std::vector<pollfd> &poll_fd, int i);
int     runPoll(Server &irc_server, std::vector<pollfd> &poll_fd);
int     runServer(Server &irc_server);
