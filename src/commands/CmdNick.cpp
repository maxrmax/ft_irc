/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdNick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:05:53 by ngoyat            #+#    #+#             */
/*   Updated: 2026/03/16 11:42:22 by mring            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

//CmdNick.cpp
#include "CmdNick.hpp"
#include "checker.hpp"
#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/


// IRC rules - RFC 1459 §2.3.1
//  First char:  letter only [A-Za-z]
//  Rest:        letters, digits, or special chars
//  Special:     - [ ] \ ` ^ { }      (pipe '|' is NOT in RFC 1459 special)
//  Max length:  no limit in RFC 1459 (9 chars is RFC 2812 — we enforce it anyway)
//  No spaces, no control characters
static bool NickIsValid(std::string nick)
{
    if (nick.empty())
        return false;

    if (nick.size() > 9)
        return false;

    // 1st is letter only
    if (!std::isalpha(nick[0]))
        return false;

    // check rest of chars
    for (unsigned long i = 1; i < nick.size(); i++)
    {
        if (!std::isalnum(nick[i]) && !isspecial_MACdarwin(nick[i]))
            return false;
    }
    return true;
};

void CmdNick::execute(Server &server, ClientUser &clientUser, const ParsedCommand &cmd)
{
    if (cmd.params.empty())
    {
        // 431     ERR_NONICKNAMEGIVEN
        clientUser.get_outputBuffer().append(":server 431 * " + cmd.command + " :No nickname given");
        return;
    }

    // validate nickname
    if (!NickIsValid(cmd.params[0]))
    {
        // 432     ERR_ERRONEUSNICKNAME
        clientUser.get_outputBuffer().append(":server 432 * " + cmd.params[0] + " :Erroneus nickname");
        return;
    }

    if (server.NickIsAlreadyRegistered(cmd.params[0]))
    {
        // 433     ERR_NICKNAMEINUSE
        clientUser.get_outputBuffer().append(":server 433 * " + cmd.params[0] + " :Nickname is already in use");
        return;
    }

    // store nickname
    std::string previouseNickname = clientUser.getNickname();
    // set new nickname for current clientUser
    clientUser.setNickname(cmd.params[0]);
    // add clientUser to server mapping based on new nickname
    server.Nick_ClientUser_mapping(clientUser);

    /* if previousNickname is different to the current nickname
     * reference the client to the old nickname
     * in the nickname_history map
     */  
    if (previouseNickname != clientUser.getNickname())
    {
        server.NicknamesHistory_storing(previouseNickname, clientUser);
    }

    // Check if ready to register after setting username and realname
    if (clientUser.isReadyToRegister() && !clientUser.isRegistered())
    {
		// Register the client with the server
        server.Nick_ClientUser_mapping(clientUser);
        clientUser.setRegistered(true);

        clientUser.get_outputBuffer().append(
            ":server 001 " + clientUser.getNickname() +
            " :Welcome to ircserver " +
            clientUser.getNickname() + "!" +
            clientUser.getUsername() + "@ircserver");
    }
}

/*
431 ERR_NONICKNAMEGIVEN
432 ERR_ERRONEUSNICKNAME
433 ERR_NICKNAMEINUSE
436 ERR_NICKCOLLISION
*/