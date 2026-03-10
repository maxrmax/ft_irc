/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 18:59:40 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/09 19:07:40 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//CmdPing.cpp

// #include "../../includes/CmdPing.hpp"
// #include "../../includes/ClientUser.hpp"
#include "../../includes/ircserv.hpp"

//client PINGs to check presence of active connection 
//Send PONG to tell client, we are still in a valid relationship
void CmdPing::execute(Server&, ClientUser& clientUser, const ParsedCommand& cmd)
{
    clientUser.get_outputBuffer().append("PONG :" + cmd.params[0] + "\r\n");
};