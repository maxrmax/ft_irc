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

// CommandDispatcher.cpp

// #include "../../includes/commandDispatcher.hpp"
// #include "../../includes/cmdNick.hpp"
// #include "../../includes/CmdUser.hpp"
// #include "../../includes/CmdJoin.hpp"
#include "../../includes/ircserv.hpp"

CommandDispatcher::CommandDispatcher()
{
    _commands["CAP"]        = new CmdCap();
    _commands["JOIN"]       = new CmdJoin();
    _commands["NICK"]       = new CmdNick();
    _commands["PASS"]       = new CmdPass();
    _commands["PING"]       = new CmdPing();
    _commands["PRIVMSG"]    = new CmdPrivmsg();
    _commands["USER"]       = new CmdUser();
    // _commands["PART"]       = new CmdPart();
    // _commands["QUIT"]       = new CmdQuit();
    // _commands["PONG"]       = new CmdPong();
    // _commands["NOTICE"]     = new CmdNotice();
    // _commands["TOPIC"]      = new CmdTopic();
    // _commands["INVITE"]      = new CmdInvite();
    // _commands["KICK"]      = new CmdKick();
    _commands["MODE"]      = new CmdMode();
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
// calls dispatcher.dispatch() with the parsed command
void CommandDispatcher::dispatch(Server& server, ClientUser& clientUser, const ParsedCommand& cmd)
{
    std::map<std::string, Command*>::iterator it;

    if (cmd.command.empty())
    {
        clientUser.get_outputBuffer().append(":server 421  :No Command\r\n");
        return;
    }

    // PASS must be provided (and correct) before any other command is processed
    // CAP is sent by irssi automatically and we don't care about it in nc (usually)
    // QUIT to disconnect anytime
    // PING PONG
    if (!clientUser.isPassAccepted() 
        && cmd.command != "PASS" 
        && cmd.command != "CAP" 
        && cmd.command != "QUIT" 
        && cmd.command != "PING")
    {
        clientUser.get_outputBuffer().append(":server 464 * :Password required\r\n");
        return;
    }

    it = _commands.find(cmd.command);
    
    if (it == _commands.end())    
    {
        //nickname or all
        std::string target = clientUser.hasNick() ? clientUser.getNickname() : "*";
        std::string msgToSend = ":server 421 " + target + " :" + cmd.command + " Unknown command\r\n";
        clientUser.get_outputBuffer().append(msgToSend);
        return;
    }


    it->second->execute(server, clientUser, cmd);
}
