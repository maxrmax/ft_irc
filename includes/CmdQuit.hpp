/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdQuit.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maxrmax <maxrmax>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:01:23 by maxrmax           #+#    #+#             */
/*   Updated: 2026/03/16 15:01:28 by maxrmax          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "command.hpp" // <string> - <vector>


/**
 * CmdQuit
 *
 * Handles the IRC QUIT command.
 * - Syntax: QUIT [message]
 * - Marks the client for disconnection and notifies joined channels of the quit.
 */
class CmdQuit : public Command
{
public:
    void execute(Server &server,
                 ClientUser &clientUser,
                 const ParsedCommand &cmd) override;
};
