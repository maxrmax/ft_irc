/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPass.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 20:55:55 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/16 13:38:48 by mring            ###   ########.fr       */
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
        // ERR_NEEDMOREPARAMS (461)
        clientUser.get_outputBuffer().append("461 PASS : " + cmd.command + " Not enough parameters\r\n");
        return;
    }
    if (cmd.params[0] != server.get_server_password())
    {
        clientUser.get_outputBuffer().append(":server 464 * :Password incorrect\r\n");
        // on wrong password clients stay connected
        // until they disconnect (or timeout? need research)
        // clientUser.setToDisconnect(true); // TODO (remove)
        return;
    }
    clientUser.setPassAccepted(true);
}

/*
461 ERR_NEEDMOREPARAMS
462 ERR_ALREADYREGISTRED

The PASS command is used to set a 'connection password'.  The
   password can and must be set before any attempt to register the
   connection is made.  Currently this requires that clients send a PASS
   command before sending the NICK/USER combination and servers *must*
   send a PASS command before any SERVER command.  The password supplied
   must match the one contained in the C/N lines (for servers) or I
   lines (for clients).  It is possible to send multiple PASS commands
   before registering but only the last one sent is used for
   verification and it may not be changed once registered.  Numeric
   Replies:

           ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED

   Example:

           PASS secretpasswordhere


OPER CMD Reply - not PASS:
464 ERR_PASSWDMISMATCH
*/