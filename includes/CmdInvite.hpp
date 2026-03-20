/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdInvite.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 17:47:13 by mring             #+#    #+#             */
/*   Updated: 2026/03/17 17:47:13 by mring            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "command.hpp"

class CmdInvite : public Command
{
public:
    void execute(Server &server, 
                 ClientUser &clientUser,
                 const ParsedCommand &cmd) override;
};
