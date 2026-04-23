/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdNotice.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngoyat <ngoyat@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 17:49:39 by ngoyat            #+#    #+#             */
/*   Updated: 2026/03/16 17:49:39 by ngoyat           ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "CmdNotice.hpp"
#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/

void CmdNotice::execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd)
{
    // Not registered -> silently ignore (no 451)
    if (!clientUser.isRegistered())
        return;

    // Missing params -> silently ignore (no 461)
    if (cmd.params.size() < 2)
        return;

    const std::string& target  = cmd.params[0];
    const std::string& text    = cmd.params[1];

    // Empty text -> silently ignore (no 412)
    if (text.empty())
        return;

    std::string prefix = ":" + clientUser.getNickname() + "!" + clientUser.getUsername() +
             "@" + clientUser.getIp();
    std::string msgOut = prefix + " NOTICE " + target + " :" + text;

    // ── Channel message ──────────────────────────────────────────────────────
    if (target[0] == '#')
    {
        // Channel doesnt exist -> silently ignore (no 403)
        if (!server.channelExists(target))
            return;

        Channel& channel = server.getChannel(target);

        // Not a member -> silently ignore (no 404)
        if (!channel.hasMember(clientUser.get_ClientUser_fd()))
            return;

        // Broadcast to every member EXCEPT the sender
        server.broadcastToChannelExcept(target, msgOut, clientUser.get_ClientUser_fd());
        return;
    }


    //private message to individual clients
    //get all targets, which are parameter[0], delimineter ','
    // std::vector <std::string> targets;
    // std::string delimineter = ",";
    // for(int targets_cnt = 0; targets_cnt < targets.size(); targets_cnt++)
    // {
    //     int position = target.find(delimineter);
    //     targets[targets_cnt] = target.substr(0, position);
    // }
    
    // ── Private message to a nick ────────────────────────────────────────────
    ClientUser* recipient = server.getClientByNick(target);

    // No such nick -> silently ignore (no 401)
    if (!recipient)
        return;

    recipient->get_outputBuffer().append(msgOut);

    // std::cout << server.getClientByNick(cmd.params[0])->get_outputBuffer().get_buffer() << std::endl;
}

/*
// 4.4.2 Notice

//       Command: NOTICE
//    Parameters: <nickname> <text>

//    The NOTICE message is used similarly to PRIVMSG.  The difference
//    between NOTICE and PRIVMSG is that automatic replies must never be
//    sent in response to a NOTICE message.  This rule applies to servers
//    too - they must not send any error reply back to the client on
//    receipt of a notice.  The object of this rule is to avoid loops
//    between a client automatically sending something in response to
//    something it received.  This is typically used by automatons (clients
//    with either an AI or other interactive program controlling their
//    actions) which are always seen to be replying lest they end up in a
//    loop with another automaton.

//    See PRIVMSG for more details on replies and examples.

// Commonly used for:

// - Server messages
// - Bot informational messages
// - NickServ / ChanServ replies
// - Warnings
// - Non-interactive notifications

// IRC NOTICE command
// Syntax: NOTICE <target> :<message>
*/