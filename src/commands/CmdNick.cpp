/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdNick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngoyat <ngoyat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:05:53 by ngoyat            #+#    #+#             */
/*   Updated: 2026/01/30 12:05:53 by ngoyat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cmdNick.hpp"
#include "../../includes/Client.hpp"

void CmdNick::execute(Server&, Client& client, const ParsedCommand& cmd)
{
    if (cmd.params.empty())
        return;
    if (client.isReadyToRegister() && !client.getNickname().empty() && !client.getUsername().empty() && client.isRegistered() == false)
    {
        client.setRegistered(true);
        // send welcome message
        return;
        // ERR_ALREADYREGISTRED
    }
// TODO: check for valid nicknames + add error handling + add to client.cpp
    // client.setNick(cmd.params[0]);
}
