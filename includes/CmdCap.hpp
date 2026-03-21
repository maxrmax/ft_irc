/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdCap.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 12:52:57 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/09 18:57:46 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "command.hpp" // <string> - <vector>

/**
 * CmdCap
 *
 * Handles the CAP (capabilities) command sequence used during client capability negotiation.
 * - Syntax: CAP <subcommand> [params]
 * - Expected to negotiate optional features; implementation may respond and modify client state.
 */
class CmdCap : public Command
{
public:
    void execute(Server &server,
                 ClientUser &client,
                 const ParsedCommand &cmd) override;
};
