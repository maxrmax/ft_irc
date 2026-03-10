/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPass.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 20:55:55 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/09 17:32:41 by nsloniow         ###   ########.fr       */
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
        //ERR_PASSWDMISMATCH
        clientUser.get_outputBuffer().append(": server 461 : " + cmd.command + "Not enough parameters\r\n");
        return;
    }
    //char serverPassword = server.get_server_password();
    // printf("%s %d server_password %s\n", __FILE__, __LINE__, serverPassword);
    // std::cout << __FILE__ << __LINE__ << std::endl << " server_password " <<server.get_server_password() << std::endl;
    if (cmd.params[0] != server.get_server_password())
    {
        // ERR_NEEDMOREPARAMS
        clientUser.get_outputBuffer().append(": server 464  :Password incorrect\r\n"); 
        return;
    }
    
    // std::cout << std::endl << __FILE__ << __LINE__ << "PassAccepted before " << clientUser.isPassAccepted() << std::endl;
    clientUser.setPassAccepted(true);
    // std::cout << std::endl << __FILE__ << __LINE__ << "PassAccepted after  " << clientUser.isPassAccepted() << std::endl;
    
    if (clientUser.isReadyToRegister() && !clientUser.isRegistered())
    {
        // std::cout << __FILE__ << __LINE__ << " PASS execute" << std::endl;
        // Register the client with the server
        server.Nick_ClientUser_mapping(clientUser);
        clientUser.setRegistered(true);

        clientUser.get_outputBuffer().append(
            ":server 001 " + clientUser.getNickname() +
            " :Welcome to ircserver " +
            clientUser.getNickname() + "!" +
            clientUser.getUsername() + "@ircserver\r\n");
    }

    // server.printRegisteredNicks();
}