/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdNick.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngoyat <ngoyat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:06:34 by ngoyat            #+#    #+#             */
/*   Updated: 2026/01/30 12:06:34 by ngoyat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "command.hpp"


/**
 * CmdNick
 *
 * Handles the IRC NICK command.
 * - Syntax: NICK <nickname>
 * - Registers/changes the client's nickname, checking for collisions and history.
 * - May complete registration when USER is present.
 */
class CmdNick : public Command
{
public:
    void execute(Server &server,
                 ClientUser &client,
                 const ParsedCommand &cmd) override;
};
