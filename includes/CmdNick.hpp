/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdNick.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:06:34 by student           #+#    #+#             */
/*   Updated: 2026/01/30 12:06:34 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "command.hpp" // <string> - <vector>


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
