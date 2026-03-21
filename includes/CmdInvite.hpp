/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdInvite.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 17:47:13 by mring             #+#    #+#             */
/*   Updated: 2026/03/17 17:47:13 by mring            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "command.hpp" // <string> - <vector>

/**
 * CmdInvite
 *
 * Handles the IRC INVITE command.
 * - Syntax: INVITE <nick> <channel>
 * - Sends an invitation to <nick> to join <channel> if caller has permission.
 * - Side effects: may append reply messages to the caller and the invited user.
 */
class CmdInvite : public Command
{
public:
    void execute(Server &server,
                 ClientUser &clientUser,
                 const ParsedCommand &cmd) override;
};
