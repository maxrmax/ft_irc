/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPass.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 20:55:55 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/10 14:47:55 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// CmdPass.cpp

#include "../../includes/ircserv.hpp"

// PASS check handled in dispatcher before any cmd can be run
// This class merely sets password accepted true
void CmdPass::execute(Server &server, ClientUser &clientUser, const ParsedCommand &cmd)
{
    if (cmd.params.empty())
    {
        // ERR_NEEDMOREPARAMS -- ERR_PASSWDMISMATCH is for OPER command, not pass
        clientUser.get_outputBuffer().append("461 PASS : " + cmd.command + " Not enough parameters\r\n");
        return;
    }
    if (cmd.params[0] != server.get_server_password())
    {
        clientUser.get_outputBuffer().append(":server 464 * :Password incorrect\r\n");
        /* TODO: proper disconnect on no/false password
         * currently clients without proper password are getting "stuck"
         * they don't disconnect and "die"
         * prototype call below: */
        // clientUser.setToDisconnect(true); // TODO
        return;
    }
    clientUser.setPassAccepted(true);
}
//     if (clientUser.isReadyToRegister() && !clientUser.isRegistered())
//     {
//         // Register the client with the server
//         server.Nick_ClientUser_mapping(clientUser);
//         clientUser.setRegistered(true);

//         clientUser.get_outputBuffer().append(
//             ":server 001 " + clientUser.getNickname() +
//             " :Welcome to ircserver " +
//             clientUser.getNickname() + "!" +
//             clientUser.getUsername() + "@ircserver\r\n");
//     }

//     // server.printRegisteredNicks();
// }
