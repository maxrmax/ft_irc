/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:11:15 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/16 14:09:42 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <set>
#include <csignal>

#include "ClientUser.hpp"
#include "Channel.hpp"
#include "CmdCap.hpp"
#include "CmdJoin.hpp"
#include "CmdKick.hpp"
#include "CmdMode.hpp"
#include "CmdNick.hpp"
#include "CmdNotice.hpp"
#include "CmdPass.hpp"
#include "CmdPart.hpp"
#include "CmdPing.hpp"
#include "CmdPrivmsg.hpp"
#include "CmdUser.hpp"
#include "commandDispatcher.hpp"
#include "inputHandling.hpp"
#include "poll.hpp"
#include "server.hpp"

//IRC standard protocol max message length 512
static const size_t IRC_MAX_LINE = 510; // excluding \r\n

//isDigit.cpp
bool    is_digits_only(const std::string &string);
bool    isspecial(int i);

//main.cpp
int     check_arguments(int argc, char **argv);
void    printCommand(const ParsedCommand& cmd, const ClientUser& clientUser);

//runServer.cpp
int     acceptClientUser(Server &irc_server, std::vector<pollfd> &poll_fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd);
int     clientUsers_waiting(Server &irc_server, std::vector<pollfd> &poll_fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd);
int     process_ready_fd(Server &irc_server, std::vector<pollfd> &poll_fd, int fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd);
int     receive_message(Server &irc_server, std::vector<pollfd> &poll_fd, int fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd);
int     runPoll(Server &irc_server, std::vector<pollfd> &poll_fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd);
int     runServer(Server &irc_server);
