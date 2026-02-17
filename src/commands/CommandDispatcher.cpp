/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandDispatcher.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngoyat <ngoyat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:06:10 by ngoyat            #+#    #+#             */
/*   Updated: 2026/01/30 12:06:10 by ngoyat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "../../includes/commandDispatcher.hpp"
// #include "../../includes/cmdNick.hpp"
// #include "../../includes/CmdUser.hpp"
// #include "../../includes/CmdJoin.hpp"
#include "../../includes/ircserv.hpp"

CommandDispatcher::CommandDispatcher()
{
    _commands["NICK"] = new CmdNick();
    // _commands["USER"] = new CmdUser();
    // _commands["JOIN"] = new CmdJoin();
}

CommandDispatcher::~CommandDispatcher()
{
    std::map<std::string, Command*>::iterator it;

    it = _commands.begin();
    while (it != _commands.end())
    {
        delete it->second;
        it++;
    }
}
// dispatch command to the right command class
// server knows all clients, channels, etc.
// server calls handleClientInput()
//calls dispatcher.dispatch() with the parsed command
void CommandDispatcher::dispatch(Server& server, ClientUser& clientUser, const ParsedCommand& cmd)
{
    std::map<std::string, Command*>::iterator it;

    if (cmd.command.empty())
        return;

    it = _commands.find(cmd.command);
    if (it == _commands.end())
        return;

    it->second->execute(server, clientUser, cmd);
}
