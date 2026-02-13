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
#include "../../includes/ClientUser.hpp"

void CmdNick::execute(Server&, ClientUser& clientUser, const ParsedCommand& cmd)
{
    if (cmd.params.empty())
        return;
// TODO: check for valid nicknames + add error handling + add to clientUser.cpp
    // clientUser.setNick(cmd.params[0]);
}
