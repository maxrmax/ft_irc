/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdInvite.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 17:47:13 by mring             #+#    #+#             */
/*   Updated: 2026/03/17 17:47:13 by mring            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "CmdInvite.hpp"
#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/

// Channel names must start with '#' and contain no spaces, NUL, BEL, or commas
static bool channelNameIsValid(const std::string& name)
{
    if (name.size() < 2 || name.size() > 50)
        return false;
    if (name[0] != '#')
        return false;
    for (size_t i = 1; i < name.size(); i++)
    {
        char c = name[i];
        if (c == ' ' || c == '\0' || c == '\a' || c == ',')
            return false;
    }
    return true;
}

void CmdInvite::execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd)
{
    /*
        flow of cmdinvite:
        INVITE <nickname> <channel>
        INVITE      cmd
        <nickname>  cmd.params[0]
        <channel>   cmd.params[1]

        first: check if cmd.params.empty()
        or
        size() < 2 (at least one variable missing) INVITE nickname -> 1 param < 2

        then check if channel name is valid (if it isn't, fail)
        if its valid -> OR if channel even exist (if it doesn't, fail)

        then we get the channel object

        check if  caller is in channel

        check if the caller is op -> op fail

        check if the target exist

        check if target is already in channel

        send reply to target
    */

    // if the params are empty
    if (cmd.params.empty() || cmd.params.size() < 2)
    {
        clientUser.get_outputBuffer().append(
            ":server 461 " + clientUser.getNickname() + " INVITE :Not enough parameters\r\n");
        return;
    }

    std::string channelName = cmd.params[1];
    std::string targetUsername = cmd.params[0];



    if (!channelNameIsValid(channelName) || !server.channelExists(channelName))
    {
        // ERR_NOSUCHCHANNEL (403) reused for invalid name here
        clientUser.get_outputBuffer().append(
            ":server 403 " + clientUser.getNickname() + " " + channelName +
            " :No such channel\r\n");
        return;
    }
    Channel& channel = server.getChannel(channelName);


    // channelmember check
    if (!channel.hasMember(clientUser.get_ClientUser_fd()))
    {
        // ERR_NOTONCHANNEL 442
        clientUser.get_outputBuffer().append(
            ":server 442 " + clientUser.getNickname() + " " + channelName +
            " :You're not on that channel\r\n");
        return;
    }

    // opcheck
    if (!channel.isOperator(clientUser.get_ClientUser_fd()))
    {
        // ERR_CHANOPRIVSNEEDED 482
        clientUser.get_outputBuffer().append(
            ":server 482 " + clientUser.getNickname() + " " + channelName +
            " :You're not channel operator\r\n");
        return;
    }

    ClientUser* target = server.getClientByNick(targetUsername);
    if (!target)
    {
        // ERR_NOSUCHNICK 401
        clientUser.get_outputBuffer().append(
            ":server 401 " + clientUser.getNickname() + " " + targetUsername +
            " :No such nick\r\n");
        return;
    }

    // If already in channel
    if (channel.hasMember(target->get_ClientUser_fd()))
    {
        // ERR_NOTONCHANNEL 443
        clientUser.get_outputBuffer().append(
            ":server 443 " + target->getNickname() + " " + channelName +
            " :is already on channel\r\n");
        return;
    }

    channel.setInvited(target->get_ClientUser_fd());

    // RPL_INVITING (341):   "<channel> <nick>"
    // clientuser or target?
    clientUser.get_outputBuffer().append(
        ":server 341 " + clientUser.getNickname() + " " + channelName +
        " :" + targetUsername + "\r\n");

    // now send the invite to the recipient
    // :inviterNick!user@host INVITE <targetNick> <#channel>
    target->get_outputBuffer().append(
        ":" + clientUser.getNickname() + "!" + clientUser.getUsername() +
        "@" + clientUser.getIp() + " INVITE " + targetUsername + " " + channelName + "\r\n");
}

/*
:server 001 n3ck :Welcome to ircserver n3ck!n3ck@ircserver

join #ch1
invite #ch1 n1ck
:server 403 n3ck n1ck :No such channel

privmsg ch test
:server 401 n3ck ch :No such nick
//correct

privmsg #ch test
:server 404 n3ck #ch :Cannot send to channel
//correct

privmsg #ch1 test
//
invite n1ck #ch1
:server 341 n3ck = #ch1 :n1ck
// = (wrong)

n1ck didn't receive a INVITE notice.
*/


/*
341 RPL_INVITING

// 401 ERR_NOSUCHNICK
// 403 ERR_NOSUCHCHANNEL
// 442 ERR_NOTONCHANNEL
// 443 ERR_USERONCHANNEL
// 461 ERR_NEEDMOREPARAMS
// 482 ERR_CHANOPRIVSNEEDED
*/


/*
// 4.2.7 Invite message

//       Command: INVITE
//    Parameters: <nickname> <channel>

//    The INVITE message is used to invite users to a channel.  The
//    parameter <nickname> is the nickname of the person to be invited to
//    the target channel <channel>.  There is no requirement that the
//    channel the target user is being invited to must exist or be a valid
//    channel.  To invite a user to a channel which is invite only (MODE
//    +i), the client sending the invite must be recognised as being a
//    channel operator on the given channel.

//    Numeric Replies:

//            ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
//            ERR_NOTONCHANNEL                ERR_USERONCHANNEL
//            ERR_CHANOPRIVSNEEDED
//            RPL_INVITING                    RPL_AWAY

//    Examples:

//    :Angel INVITE Wiz #Dust         ; User Angel inviting WiZ to channel
//                                    #Dust

//    INVITE Wiz #Twilight_Zone       ; Command to invite WiZ to
//                                    #Twilight_zone

*/