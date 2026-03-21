/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdNotice.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngoyat <ngoyat@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 17:50:39 by ngoyat            #+#    #+#             */
/*   Updated: 2026/03/16 17:50:39 by ngoyat           ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "command.hpp" // <string> - <vector>


/**
 * CmdNotice
 *
 * Handles the IRC NOTICE command.
 * - Syntax: NOTICE <target> :<message>
 * - Sends a message to a target (user or channel) without automatic replies (no ERR_NORECIPIENT etc.).
 */
class CmdNotice : public Command
{
public:
    void execute(Server &server,
                 ClientUser &clientUser,
                 const ParsedCommand &cmd) override;
};
