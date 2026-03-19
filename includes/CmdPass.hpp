/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPass.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 20:50:35 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/10 17:29:07 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "command.hpp"


/**
 * CmdPass
 *
 * Handles the IRC PASS command.
 * - Syntax: PASS <password>
 * - Provides a password used during initial registration; may set authentication state.
 */
class CmdPass : public Command
{
public:
    void execute(Server &server,
                 ClientUser &clientUser,
                 const ParsedCommand &cmd) override;
};
