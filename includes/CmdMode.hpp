/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdMode.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:31:28 by mring             #+#    #+#             */
/*   Updated: 2026/03/16 11:48:41 by mring            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "command.hpp" // <string> - <vector>


/**
 * CmdMode
 *
 * Handles the IRC MODE command for channels and users.
 * - Syntax (channel): MODE <channel> {[+|-]|o|t|i|k|l} [params]
 * - Modifies channel/user modes; enforces permission checks and updates channel state.
 */
class CmdMode : public Command
{
public:
    void execute(Server &server,
                 ClientUser &clientUser,
                 const ParsedCommand &cmd) override;
};
