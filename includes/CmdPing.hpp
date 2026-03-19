/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 18:57:34 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/09 19:06:45 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "command.hpp"

/**
 * CmdPing
 *
 * Handles the IRC PING command.
 * - Syntax: PING <server1> [server2]
 * - Replies with PONG to keep connections alive and measure latency.
 */
class CmdPing : public Command
{
public:
    void execute(Server &server,
                 ClientUser &client,
                 const ParsedCommand &cmd) override;
};
