/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdTopic.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 18:39:39 by ngoyat            #+#    #+#             */
/*   Updated: 2026/04/23 11:05:06 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CmdTopic.hpp"
#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/

void CmdTopic::execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd)
{
    if (!clientUser.isRegistered())
    {
        clientUser.get_outputBuffer().append(
            ":server 451 * :You have not registered");
        return;
    }

    // TOPIC requires at least a channel name
    if (cmd.params.empty())
    {
        clientUser.get_outputBuffer().append(
            ":server 461 " + clientUser.getNickname() + " TOPIC :Not enough parameters");
        return;
    }

    const std::string& channelName = cmd.params[0];

    if (!server.channelExists(channelName))
    {
        // ERR_NOSUCHCHANNEL
        clientUser.get_outputBuffer().append(
            ":server 403 " + clientUser.getNickname() + " " + channelName +
            " :No such channel");
        return;
    }

    Channel& channel = server.getChannel(channelName);

    // Must be a member to query or set topic
    if (!channel.hasMember(clientUser.get_ClientUser_fd()))
    {
        // ERR_NOTONCHANNEL
        clientUser.get_outputBuffer().append(
            ":server 442 " + clientUser.getNickname() + " " + channelName +
            " :You're not on that channel");
        return;
    }

    // ── Query: TOPIC #channel (no second param) ───────────────────────────────
    if (cmd.params.size() == 1)
    {
        if (channel.getTopic().empty())
        {
            // RPL_NOTOPIC
            clientUser.get_outputBuffer().append(
                ":server 331 " + clientUser.getNickname() + " " + channelName +
                " :No topic is set");
        }
        else
        {
            // RPL_TOPIC
            clientUser.get_outputBuffer().append(
                ":server 332 " + clientUser.getNickname() + " " + channelName +
                " :" + channel.getTopic());
        }
        return;
    }

    // ── Set / clear: TOPIC #channel :<text> ──────────────────────────────────

    // If +t flag is set, only operators may change the topic
    if (channel.getTopicFlag() && !channel.isOperator(clientUser.get_ClientUser_fd()))
    {
        // ERR_CHANOPRIVSNEEDED
        clientUser.get_outputBuffer().append(
            ":server 482 " + clientUser.getNickname() + " " + channelName +
            " :You're not channel operator");
        return;
    }

    const std::string& newTopic = cmd.params[1];
    channel.setTopic(newTopic);

    // Broadcast the topic change to all members
    std::string prefix = ":" + clientUser.getNickname() + "!" +
                         clientUser.getUsername() + "@" + clientUser.getIp();
    std::string broadcast = prefix + " TOPIC " + channelName + " :" + newTopic;
    server.broadcastToChannel(channelName, broadcast);
}

/*
// 4.2.4 Topic message

//       Command: TOPIC
//    Parameters: <channel> [<topic>]

//    The TOPIC message is used to change or view the topic of a channel.
//    The topic for channel <channel> is returned if there is no <topic>
//    given.  If the <topic> parameter is present, the topic for that
//    channel will be changed, if the channel modes permit this action.

//    Numeric Replies:

//            ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
//            RPL_NOTOPIC                     RPL_TOPIC
//            ERR_CHANOPRIVSNEEDED



// Oikarinen & Reed                                               [Page 23]

// RFC 1459              Internet Relay Chat Protocol              May 1993


//    Examples:
//    :Wiz TOPIC #test :New topic     ;User Wiz setting the topic.
//    TOPIC #test :another topic      ;set the topic on #test to "another
//                                    topic".
//    TOPIC #test                     ; check the topic for #test.


// RFC 1459 - 4.2.4 Topic

// Syntax:  TOPIC <channel>            → query current topic
//          TOPIC <channel> :          → clear topic
//          TOPIC <channel> :<topic>   → set new topic

// _topicFlag (channel mode +t):
//   true  → only operators can change the topic
//   false → any member can change the topic
*/
