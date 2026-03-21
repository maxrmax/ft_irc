/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPart.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 13:17:01 by mring             #+#    #+#             */
/*   Updated: 2026/03/16 13:17:14 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "command.hpp" // <string> - <vector>


/**
 * CmdPart
 *
 * Handles the IRC PART command.
 * - Syntax: PART <channel> [reason]
 * - Removes caller from the specified channel and notifies members.
 */
class CmdPart : public Command
{
public:
    void execute(Server &server,
                 ClientUser &clientUser,
                 const ParsedCommand &cmd) override;
};
