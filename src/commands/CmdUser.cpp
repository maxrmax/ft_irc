/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdUser.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 01:22:33 by ngoyat            #+#    #+#             */
/*   Updated: 2026/02/24 10:37:04 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/CmdUser.hpp"
#include "../../includes/ClientUser.hpp"

//IRC USER syntax: USER <username> <hostname> <servername> :<realname>
void CmdUser::execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd)
{
    if (clientUser.isRegistered())
    {
        clientUser.get_outputBuffer().append(
            ":server 462 :You may not reregister\r\n");
        return;
    }

    if (cmd.params.size() < 4)
    {
        clientUser.get_outputBuffer().append(
            ":server 461 USER :Not enough parameters\r\n");
        return;
    }

    std::string username = cmd.params[0];
    std::string realname = cmd.params[3];

    clientUser.setUsername(username);
    clientUser.setRealname(realname);

    // Check if ready to register after setting username and realname
    if (clientUser.isReadyToRegister() &&
        !clientUser.getNickname().empty() &&
        !clientUser.getUsername().empty() &&
        !clientUser.isRegistered())
    {
		// Register the client with the server
        server.Nick_ClientUser_mapping(clientUser);
        clientUser.setRegistered(true);

        clientUser.get_outputBuffer().append(
            ":server 001 " + clientUser.getNickname() +
            " :Welcome to ircserver " +
            clientUser.getNickname() + "!" +
            clientUser.getUsername() + "@ircserver\r\n");
    }
}