/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdJoin.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 14:06:25 by ngoyat            #+#    #+#             */
/*   Updated: 2026/03/16 13:33:45 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CmdJoin.hpp"
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

static std::vector<std::string> split_elements(const std::string &input)
{
    std::vector<std::string> elements;
    size_t start = 0;
    /* loop flow
     * while true:
     * find the next comma position in the input string
     * if: comma is not found in the remaining string
     * push_back the element from the position start
     * 
     * create a substr from start until the next position (comma-start)
     * and push it onto the elements vectorstack (#ch1,#ch2,#ch3 -> [0]=#ch1 [1]=#ch2 [2]=#ch3)
     * key1,,key3 -> [0]=key1 [1]="" [2]=key3?
    */
    while (true)
    {
        size_t comma = input.find(',', start);
        if (comma == std::string::npos)
        {
            elements.push_back(input.substr(start));
            return elements;
        }
        elements.push_back(input.substr(start, comma - start));
        start = comma + 1;
    }
    return elements;
}

void CmdJoin::execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd)
{
    // Must be registered to JOIN
    if (!clientUser.isRegistered())
    {
        clientUser.get_outputBuffer().append(
            ":server 451 * :You have not registered\r\n");
        return;
    }

    // if the params are empty
    if (cmd.params.empty())
    {
        clientUser.get_outputBuffer().append(
            ":server 461 " + clientUser.getNickname() + " JOIN :Not enough parameters\r\n");
        return;
    }

    ////////////////////////////////////////////////
    // IRC allows joining multiple channels: JOIN #a,#b,#c key1,key2,key3
    // join #a,#b,#c ,key#b, (works for if only #b has a key)
    // We split on commas
    std::vector<std::string> channelNames = split_elements(cmd.params[0]);
    std::vector<std::string> keys = std::vector<std::string>();
    if (cmd.params.size() > 1 && !cmd.params[1].empty())
        keys = split_elements(cmd.params[1]);

    // build channel + key vector list after split
    std::vector<std::pair<std::string, std::string>> channels;
    for (size_t i = 0; i < channelNames.size(); i++)
    {
        std::string providedKey = (i < keys.size()) ? keys[i] : "";
        channels.push_back({channelNames[i], providedKey});
    }
    ////////////////////////////////////////////////

    // loop through the channel list vector
    for (const auto &[channelName, providededKey] : channels)
    {
        if (!channelNameIsValid(channelName))
        {
            // ERR_NOSUCHCHANNEL (403) reused for invalid name here
            clientUser.get_outputBuffer().append(
                ":server 403 " + clientUser.getNickname() + " " + channelName +
                " :No such channel (invalid name)\r\n");
            continue;
        }

        if (server.channelExists(channelName))
        {

            Channel& channel = server.getChannel(channelName);

            // If already in channel, silently skip
            if (channel.hasMember(clientUser.get_ClientUser_fd()))
                continue;

            // ORDER: i k l
            // +i are you invited?              yes/no/not needed
            // +k do you have the password?     yes/no/not needed
            // +l is the room full?             yes/no
            // check if channel is invite only && if the user is invited
            if (channel.isInviteOnly() == true && !channel.isInvited(clientUser.get_ClientUser_fd()))
            {   // ERR_INVITEONLYCHAN (473)
                clientUser.get_outputBuffer().append(
                    ":server 473 " + clientUser.getNickname() + " " + channelName +
                    " :Cannot join channel (+i)\r\n");
                continue;
            }

            // if the key IS NOT the same as the key set in the channel
            // AND key IS NOT empty
            // false = pkey != channelkey && channelkey is empty
            if ((providededKey != channel.getKey()) && !channel.getKey().empty())
            {   // ERR_BADCHANNELKEY (475)
                clientUser.get_outputBuffer().append(
                    ":server 475 " + clientUser.getNickname() + " " + channelName +
                    " :Cannot join channel (+k)\r\n");
                continue;
            }


            // if +l is set and its full
            if ((channel.getMembers().size() >= channel.getUserLimit()) && channel.getUserLimit() != 0)
            {   // ERR_CHANNELISFULL (471)
                clientUser.get_outputBuffer().append(
                    ":server 471 " + clientUser.getNickname() + " " + channelName +
                    " :Cannot join channel (+l)\r\n");
                continue;
            }

            channel.addMember(clientUser.get_ClientUser_fd());
            channel.unsetInvited(clientUser.get_ClientUser_fd());
        }
        server.createChannel(channelName, clientUser);
        Channel& channel = server.getChannel(channelName);

        // Build the JOIN prefix: :nick!user@host
        std::string prefix = ":" + clientUser.getNickname() + "!" +
                             clientUser.getUsername() + "@ircserver";

        // Broadcast JOIN to all members (including the joining user)
        std::string joinMsg = prefix + " JOIN " + channelName + "\r\n";
        server.broadcastToChannel(channelName, joinMsg);

        // Send no-topic (331) or  topic (332)
        if (channel.getTopic().empty())
        {
            clientUser.get_outputBuffer().append(
                ":server 331 " + clientUser.getNickname() + " " + channelName +
                " :No topic is set\r\n");
        }
        else
        {
            clientUser.get_outputBuffer().append(
                ":server 332 " + clientUser.getNickname() + " " + channelName +
                " :" + channel.getTopic() + "\r\n");
        }

        // RPL_NAMREPLY (353): send member list
        std::string namesList = server.getChannelMemberNicks(channelName);
        clientUser.get_outputBuffer().append(
            ":server 353 " + clientUser.getNickname() + " = " + channelName +
            " :" + namesList + "\r\n");

        // RPL_ENDOFNAMES (366)
        clientUser.get_outputBuffer().append(
            ":server 366 " + clientUser.getNickname() + " " + channelName +
            " :End of /NAMES list\r\n");
    }
}


/*
331 RPL_NOTOPIC
332 RPL_TOPIC
353 RPL_NAMREPLY
366 RPL_ENDOFNAMES	

403 ERR_NOSUCHCHANNEL
405 ERR_TOOMANYCHANNELS
471 ERR_CHANNELISFULL
473 ERR_INVITEONLYCHAN
474 ERR_BANNEDFROMCHAN
475 ERR_BADCHANNELKEY
476 ERR_BADCHANMASK
*/