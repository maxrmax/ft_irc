/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPass.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 20:55:55 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/24 10:33:01 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//CmdPass.cpp

#include "../../includes/ircserv.hpp"

//DOJO
//tore nick and user as last nick and user send until pass arrives and user can be saved and registered
void CmdPass::execute(Server &server, ClientUser &clientUser, const ParsedCommand &cmd)
{

    if (cmd.params.empty())
    {
        //ERR_NEEDMOREPARAMS
        clientUser.get_outputBuffer().append("461 PASS :Not enough parameters\r\n"); 
        return
    }
    if (cmd.params[0] != server.get_server_password())
    {
        // ERR_NEEDMOREPARAMS
        clientUser.get_outputBuffer().append(": server 461 :Not enough parameters\r\n");
        return;
    }
    else
    {
        if (clientUser.hasNick())
        {
            if (!server.NickIsAlreadyRegistered(clientUser.getNickname()))
            {
                server.Nicknames_storing(clientUser.getNickname());
                clientUser.setPassAccepted();
            }
        }
    }
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