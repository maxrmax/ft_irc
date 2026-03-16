/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPrivmsg.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 14:03:39 by ngoyat            #+#    #+#             */
/*   Updated: 2026/03/16 11:51:04 by mring            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "command.hpp"

class CmdPrivmsg : public Command
{
public:
    void execute(Server &server,
                 ClientUser &clientUser,
                 const ParsedCommand &cmd) override;
};
