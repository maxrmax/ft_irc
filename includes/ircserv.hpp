/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:11:15 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/21 12:18:07 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <algorithm>
#include <csignal>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <unordered_map>

#include "Channel.hpp"
#include "ClientUser.hpp"
#include "CmdCap.hpp"
#include "CmdInvite.hpp"
#include "CmdJoin.hpp"
#include "CmdKick.hpp"
#include "CmdMode.hpp"
#include "CmdNick.hpp"
#include "CmdNotice.hpp"
#include "CmdPass.hpp"
#include "CmdPart.hpp"
#include "CmdPing.hpp"
#include "CmdPrivmsg.hpp"
#include "CmdQuit.hpp"
#include "CmdTopic.hpp"
#include "CmdUser.hpp"
#include "commandDispatcher.hpp"
#include "inputHandling.hpp"
#include "poll.hpp"
#include "server.hpp"

/**
 * Project umbrella header
 *
 * `ircserv.hpp` is the central include that exposes core project types and
 * shared helpers to the rest of the codebase: `ClientUser`, `Channel`, all
 * `Cmd*` handlers, `Parser`, `Server` and small utility functions used
 * across translation units.
 *
 * Keep this header lightweight: prefer forward-declarations in implementation
 * headers when possible to reduce build-time coupling.
 */

// IRC standard protocol max message length 512 (including CRLF)
// Store 510 here to represent maximum content bytes excluding CRLF.
static const size_t IRC_MAX_LINE = 510; // excluding \r\n

//isDigit.cpp
bool    is_digits_only(const std::string &string);
bool    isspecial(int i);

//main.cpp
int     check_arguments(int argc, char **argv);
void    printCommand(const ParsedCommand& cmd, const ClientUser& clientUser);

//runServer.cpp
// int     acceptClientUser(Server &irc_server, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd);
int     acceptClientUser(Server &irc_server);
// int     clientUsers_waiting(Server &irc_server, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd);
int     clientUsers_waiting(Server &irc_server);
// int     process_ready_fd(Server &irc_server, int fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd);
int     process_ready_fd(Server &irc_server, int fd);
// int     receive_message(Server &irc_server, int fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd);
int     receive_message(Server &irc_server, int fd);
// int     runPoll(Server &irc_server, std::vector<pollfd> &poll_fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd);
int     runPoll(Server &irc_server, std::vector<pollfd> &poll_fd);
void    runServer(Server &irc_server);
