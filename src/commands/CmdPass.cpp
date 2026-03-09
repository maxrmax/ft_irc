/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPass.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 20:55:55 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/09 21:03:19 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// CmdPass.cpp

#include "../../includes/ircserv.hpp"

// DOJO
// tore nick and user as last nick and user send until pass arrives and user can be saved and registered
void CmdPass::execute(Server &server, ClientUser &clientUser, const ParsedCommand &cmd)
{
    // PASS check not done properly.
    // giving no password doesn't result in this empty check.
    // order:
    // PASS message
    // NICK message
    // USER message
    if (cmd.params.empty())
    {
        // ERR_NEEDMOREPARAMS
        clientUser.get_outputBuffer().append("461 PASS : " + cmd.command + " Not enough parameters\r\n");
        /* TODO: proper disconnect on no/false password
         * currently clients without proper password are getting "stuck"
         * they don't disconnect and "die"
         * prototype call below: */
        // clientUser.setToDisconnect(true); // TODO

        return;
    }
    // char serverPassword = server.get_server_password();
    //  printf("%s %d server_password %s\n", __FILE__, __LINE__, serverPassword);
    //  std::cout << __FILE__ << __LINE__ << std::endl << " server_password " <<server.get_server_password() << std::endl;
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

// const std::string& password = params[0];

// if (password == _server.get_server_password())
// {
//     client.setPassAccepted(true);

//     // optional: if the client already sent NICK/USER, you can try complete registration here
//     client.sendMessage("001 :Password accepted\r\n");
// }
// else
// {
//     client.sendMessage("464 :Password incorrect\r\n"); // ERR_PASSWDMISMATCH
//     client.disconnect(); // or close connection
// }