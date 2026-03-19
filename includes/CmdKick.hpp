/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdKick.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 11:50:03 by mring             #+#    #+#             */
/*   Updated: 2026/03/16 11:50:09 by mring            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "command.hpp"


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
