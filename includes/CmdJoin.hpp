/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdJoin.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maxrmax <maxrmax>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 14:08:56 by student           #+#    #+#             */
/*   Updated: 2026/03/16 11:48:15 by maxrmax          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "command.hpp" // <string> - <vector>

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
