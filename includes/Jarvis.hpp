/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Jarvis.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 18:37:39 by student           #+#    #+#             */
/*   Updated: 2026/03/17 18:37:39 by student          ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "command.hpp"

class CmdJarvis : public Command
{
public:
    // Called by dispatcher: /quote JARVIS time -> /quote to get arounbd irssi command filter and send raw irc command
    void execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd) override;
 
    // Called by CmdPrivmsg when text starts with "@jarvis"
    static void handleMessage(ClientUser& clientUser, const std::string& channel, const std::string& text);
};
