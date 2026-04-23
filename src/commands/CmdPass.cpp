/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPass.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 20:55:55 by nsloniow          #+#    #+#             */
/*   Updated: 2026/04/23 11:05:06 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// CmdPass.cpp

#include "CmdPass.hpp"
#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/

void CmdPass::execute(Server &server, ClientUser &clientUser, const ParsedCommand &cmd)
{
    if (cmd.params.empty())
    {
        // ERR_NEEDMOREPARAMS (461)
        clientUser.get_outputBuffer().append("461 PASS : " + cmd.command + " Not enough parameters");
        return;
    }
    if (cmd.params[0] != server.get_server_password())
    {
        clientUser.get_outputBuffer().append(":server 464 * :Password incorrect");
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