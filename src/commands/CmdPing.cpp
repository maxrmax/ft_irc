/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 18:59:40 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/16 11:42:35 by mring            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

//CmdPing.cpp

#include "../../includes/ircserv.hpp"

//client PINGs to check presence of active connection 
//Send PONG to tell client, we are still in a valid relationship
void CmdPing::execute(Server&, ClientUser& clientUser, const ParsedCommand& cmd)
{
    clientUser.get_outputBuffer().append("PONG :\r\n");
};


// server
// Command: [PRIVMSG]
// Params:
//   [0] -> [#channel]
//   [1] -> [PING 1773164358 118796]


// nc
// ping -> PONG :


// irssi
// [ctcp(#channel)] PING 1773164304 581680
//:mari!IRCTESTER@ircserver PRIVMSG nickie :PING 1773164502 885810
// ping from mari to nickie sends privmsg (normal?)
// Command: [PRIVMSG]
// Params:
//   [0] -> [nickie]
//   [1] -> [PING 1773164502 885810]
// ----------------------