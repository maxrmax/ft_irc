/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPrivmsg.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 19:25:39 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/16 14:38:45 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//CmdPrivmsg.cpp


// IRC PRIVMSG command
// Syntax: PRIVMSG <target> :<message>
// Target can be a #channel or a nickname

#include "../../includes/ircserv.hpp"

void CmdPrivmsg::execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd)
{
    // Must be registered
    if (!clientUser.isRegistered())
    {
        clientUser.get_outputBuffer().append(
            ":server 451 * :You have not registered\r\n");
        return;
    }

    // PRIVMSG <target> :<text>  → params[0] = target, params[1] = text
    if (cmd.params.size() < 2)
    {
        clientUser.get_outputBuffer().append(
            ":server 461 " + clientUser.getNickname() + " PRIVMSG :Not enough parameters\r\n");
        return;
    }

    const std::string& target  = cmd.params[0];
    const std::string& text    = cmd.params[1];

    if (text.empty())
    {
        clientUser.get_outputBuffer().append(
            ":server 412 " + clientUser.getNickname() + " :No text to send\r\n");
        return;
    }

    //start nsloniow2603121042
    //instead of using string "ircserver", we use ip, provided by client 
    //on command USER with 2nd parameter
    // std::string prefix = ":" + clientUser.getNickname() + "!" +
    //                      clientUser.getUsername() + "@ircserver";
    std::string prefix = ":" + clientUser.getNickname() + "!" + clientUser.getUsername() +
             "@" + clientUser.getIp();
    //end nsloniow2603121042
    std::string msgOut = prefix + " PRIVMSG " + target + " :" + text + "\r\n";

    // ── Channel message ──────────────────────────────────────────────────────
    if (target[0] == '#')
    {
        if (!server.channelExists(target))
        {
            clientUser.get_outputBuffer().append(
                ":server 403 " + clientUser.getNickname() + " " + target +
                " :No such channel\r\n");
            return;
        }

        Channel& channel = server.getChannel(target);
        if (!channel.hasMember(clientUser.get_ClientUser_fd()))
        {
            // ERR_CANNOTSENDTOCHAN
            clientUser.get_outputBuffer().append(
                ":server 404 " + clientUser.getNickname() + " " + target +
                " :Cannot send to channel\r\n");
            return;
        }

        // Broadcast to every member EXCEPT the sender
        server.broadcastToChannelExcept(target, msgOut, clientUser.get_ClientUser_fd());
        return;
    }


    //private message to individual clients
    //get all targets, which are parameter[0], delimineter ','
    std::vector <std::string> targets;
    std::string delimineter = ",";
    for(int targets_cnt = 0; targets_cnt < targets.size(); targets_cnt++)
    {
        int position = target.find(delimineter);
        targets[targets_cnt] = target.substr(0, position);
    }
    
    // ── Private message to a nick ────────────────────────────────────────────
    ClientUser* recipient = server.getClientByNick(target);
    if (!recipient)
    {
        // ERR_NOSUCHNICK
        clientUser.get_outputBuffer().append(
            ":server 401 " + clientUser.getNickname() + " " + target +
            " :No such nick\r\n");
        return;
    }
    
    recipient->get_outputBuffer().append(msgOut);
    
    std::cout << server.getClientByNick(cmd.params[0])->get_outputBuffer().get_buffer() << std::endl;
}


/*
401 ERR_NOSUCHNICK
404 ERR_CANNOTSENDTOCHAN
411 ERR_NORECIPIENT //
412 ERR_NOTEXTTOSEND
407 ERR_TOOMANYTARGETS //
413 ERR_NOTOPLEVEL //
414 ERR_WILDTOPLEVEL //
*/

/*
4.4.1 Private messages

      Command: PRIVMSG
   Parameters: <receiver>{,<receiver>} <text to be sent>

   PRIVMSG is used to send private messages between users.  <receiver>
   is the nickname of the receiver of the message.  <receiver> can also
   be a list of names or channels separated with commas.

   The <receiver> parameter may also me a host mask  (#mask)  or  server
   mask  ($mask).   In  both cases the server will only send the PRIVMSG
   to those who have a server or host matching the mask.  The mask  must
   have at  least  1  (one)  "."  in it and no wildcards following the
   last ".".  This requirement exists to prevent people sending messages
   to  "#*"  or "$*",  which  would  broadcast  to  all  users; from
   experience, this is abused more than used responsibly and properly.
   Wildcards are  the  '*' and  '?'   characters.   This  extension  to
   the PRIVMSG command is only available to Operators.

   Numeric Replies:

           ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
           ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
           ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
           ERR_NOSUCHNICK
           RPL_AWAY

   Examples:

:Angel PRIVMSG Wiz :Hello are you receiving this message ?
                                ; Message from Angel to Wiz.

PRIVMSG Angel :yes I'm receiving it !receiving it !'u>(768u+1n) .br ;
                                Message to Angel.

PRIVMSG jto@tolsun.oulu.fi :Hello !
                                ; Message to a client on server
                                tolsun.oulu.fi with username of "jto".

PRIVMSG $*.fi :Server tolsun.oulu.fi rebooting.
                                ; Message to everyone on a server which
                                has a name matching *.fi.

PRIVMSG #*.edu :NSFNet is undergoing work, expect interruptions
                                ; Message to all users who come from a
                                host which has a name matching *.edu.

*/