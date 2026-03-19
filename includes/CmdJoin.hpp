/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdJoin.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 14:08:56 by ngoyat            #+#    #+#             */
/*   Updated: 2026/03/16 11:48:15 by mring            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "command.hpp"

/**
 * CmdJoin
 *
 * Handles the IRC JOIN command.
 * - Syntax: JOIN <channel> [key]
 * - Adds the caller to the channel, creates the channel if it does not exist,
 *   and enforces invite/key/limit modes.
 */
class CmdJoin : public Command
{
public:
    void execute(Server &server,
                 ClientUser &clientUser,
                 const ParsedCommand &cmd) override;
};
