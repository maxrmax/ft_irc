/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleClientInput.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 12:57:30 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/18 03:57:06 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//handleClientInput.cpp

#include "../../includes/ircserv.hpp"

void handleClientInput(ClientUser &clientUser, Server &irc_server)
{
    while (clientUser.get_inputBuffer().hasLine())
    {
        //parse msg for command
        ParsedCommand cmd = Parser::parseLine(clientUser.get_inputBuffer().popLine());
        printCommand(cmd, clientUser);
        irc_server.get_dispatcher().dispatch(irc_server, clientUser, cmd);
    }
}