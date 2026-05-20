/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdKick.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maxrmax <maxrmax>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 11:50:03 by maxrmax           #+#    #+#             */
/*   Updated: 2026/03/16 11:50:09 by maxrmax          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "command.hpp" // <string> - <vector>


/**
 * CmdKick
 *
 * Handles the IRC KICK command.
 * - Syntax: KICK <channel> <user> [reason]
 * - Removes a user from a channel; requires operator privileges.
 */
class CmdKick : public Command
{
public:
    void execute(Server &server,
                 ClientUser &client,
                 const ParsedCommand &cmd) override;
};
