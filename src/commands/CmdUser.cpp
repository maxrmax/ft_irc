/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdUser.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 01:22:33 by ngoyat            #+#    #+#             */
/*   Updated: 2026/03/10 14:57:55 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// CmdUser.cpp
//  #include "../../includes/CmdUser.hpp"
//  #include "../../includes/ClientUser.hpp"
#include "../../includes/ircserv.hpp"

// IRC USER syntax: USER <username> <hostname> <servername> :<realname>
void CmdUser::execute(Server &server, ClientUser &clientUser, const ParsedCommand &cmd)
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

    // Check if ready to register after setting nickname and username
    // USER firstname hostname servername :Real Name
    // (: means "rest of line", its defined in parser.cpp)
    if (clientUser.isReadyToRegister() && !clientUser.isRegistered())
    {
        clientUser.setRegistered(true);

        server.Nick_ClientUser_mapping(clientUser);

        server.printRegisteredNicks();

        clientUser.get_outputBuffer().append(
            ":server 001 " + clientUser.getNickname() +
            " :Welcome to ircserver " +
            clientUser.getNickname() + "!" +
            clientUser.getUsername() + "@ircserver\r\n");
    }
}
