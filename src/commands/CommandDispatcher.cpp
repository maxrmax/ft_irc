/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandDispatcher.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:06:10 by ngoyat            #+#    #+#             */
/*   Updated: 2026/03/16 12:11:21 by mring            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp
#include "poll.hpp"                 // <poll.h>   - <vector>
#include "commandDispatcher.hpp"    // <map>      - <string>
#include "Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
#include "ClientUser.hpp"           // <string>
*/
#include "Parser.hpp"

#include "CmdCap.hpp"
#include "CmdJoin.hpp"
#include "CmdNick.hpp"
#include "CmdPass.hpp"
#include "CmdPing.hpp"
#include "CmdPrivmsg.hpp"
#include "CmdUser.hpp"
#include "CmdPart.hpp"
#include "CmdQuit.hpp"
#include "CmdNotice.hpp"
#include "CmdTopic.hpp"
#include "CmdInvite.hpp"
#include "CmdKick.hpp"
#include "CmdMode.hpp"


CommandDispatcher::CommandDispatcher()
{
    _commands["CAP"]        = new CmdCap();         // server capabilities, for irssi (not a normal command)
    _commands["JOIN"]       = new CmdJoin();        // join a channel. if channel doesn't exist -> create it and become op
    _commands["NICK"]       = new CmdNick();        // change nickname to a currently non registered nickname -> save history reference
    _commands["PASS"]       = new CmdPass();        // password for the server to connect to. necessary to be able to interact with the server beyond read only information.
    _commands["PING"]       = new CmdPing();        // keep alive method between client-server
    _commands["PRIVMSG"]    = new CmdPrivmsg();     // send private messages between connected clients on a server. Can be channels! PRIVMSG <#channel>,<nick> <text to be sent> -> all users of #channel AND nick would receive the message!   
    _commands["USER"]       = new CmdUser();        // used to register: USER <username> <hostname> <servername> <realname> // HOSTNAME AND SERVERNAME ARE IGNORED! (by default) // need to figure out an alternate for Privmsg!
    _commands["PART"]       = new CmdPart();        // disconnect from a channel: PART #channel1,#channel2,#channel3
    _commands["QUIT"]       = new CmdQuit();        // gracefully disconnect from a server with instant cleanup
    _commands["NOTICE"]     = new CmdNotice();      // send notice to 
    _commands["TOPIC"]      = new CmdTopic();       // needs to check if +t is set (op only) then check if op
    _commands["INVITE"]     = new CmdInvite();      // can invite any user to any channel (even non existing). If channel has +i flag, needs op. INVITE <nickname> <#channel>
    _commands["KICK"]       = new CmdKick();        // needs op - forcefully part user from channel. KICK <#channel> <nickname> <comment>
    _commands["MODE"]       = new CmdMode();        // channel settings (needs operator)
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
        std::string target = clientUser.hasNick() ? clientUser.getNickname() : "*";
        clientUser.get_outputBuffer().append(":server 421 " + target + " :No Command\r\n");
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
        std::string msgToSend = ":server 421 " + target + " " + cmd.command + " :Unknown command\r\n";
        clientUser.get_outputBuffer().append(msgToSend);
        return;
    }


    it->second->execute(server, clientUser, cmd);
}
