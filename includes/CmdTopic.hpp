/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdTopic.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 18:39:39 by student           #+#    #+#             */
/*   Updated: 2026/03/16 18:39:39 by student          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "command.hpp" // <string> - <vector>


/**
 * CmdTopic
 *
 * Handles the IRC TOPIC command.
 * - Syntax: TOPIC <channel> [topic]
 * - Gets or sets the channel topic depending on presence of the topic parameter.
 * - Enforces +t (topic protection) where appropriate.
 */
class CmdTopic : public Command
{
public:
    void execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd) override;
};
