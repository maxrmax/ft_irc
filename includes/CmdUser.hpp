/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdUser.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 01:29:04 by ngoyat            #+#    #+#             */
/*   Updated: 2026/03/03 18:50:51 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "command.hpp" // <string> - <vector>

/**
 * CmdUser
 *
 * Handles the IRC USER command used during registration.
 * - Syntax: USER <username> <hostname> <servername> :<realname>
 * - Sets user identity fields required for registration and may complete registration when NICK is present.
 */
class CmdUser : public Command
{
public:
    void execute(Server &server,
                 ClientUser &clientUser,
                 const ParsedCommand &cmd) override;
};
