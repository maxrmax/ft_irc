/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inputHandling.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 12:53:40 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/17 12:53:49 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ircserv.hpp"

/**
 * handleClientInput
 *
 * Top-level helper invoked when a client's socket has readable data.
 * Responsibilities:
 * - read from the client's socket into the client's InputBuffer
 * - parse complete lines into ParsedCommand using `Parser`
 * - dispatch parsed commands via the server's CommandDispatcher
 * - set appropriate flags on the `ClientUser` (e.g. toDisconnect)
 *
 * Side-effects: may append replies to the client's `OutputBuffer` or
 * modify server state via command handlers.
 */
void handleClientInput(ClientUser &clientUser, Server &irc_server);