/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdJoin.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 14:06:25 by ngoyat            #+#    #+#             */
/*   Updated: 2026/03/10 15:26:37 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// IRC JOIN command
// Syntax: JOIN <#channel>[,<#channel2>,...] [<key>[,<key2>,...]]

#include "../../includes/CmdJoin.hpp"
#include "../../includes/ClientUser.hpp"

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

void CmdJoin::execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd)
{
    // Must be registered to JOIN
    if (!clientUser.isRegistered())
    {
        clientUser.get_outputBuffer().append(
            ":server 451 * :You have not registered\r\n");
        return;
    }

    if (cmd.params.empty())
    {
        clientUser.get_outputBuffer().append(
            ":server 461 " + clientUser.getNickname() + " JOIN :Not enough parameters\r\n");
        return;
    }

    ////////////////////////////////////////////////
    // IRC allows joining multiple channels: JOIN #a,#b,#c key1,key2,key3
    // We split on commas
    std::string channelParam = cmd.params[0];
    std::vector<std::string> channelNames;
    {
        size_t start = 0;
        size_t comma;
        while ((comma = channelParam.find(',', start)) != std::string::npos)
        {
            channelNames.push_back(channelParam.substr(start, comma - start));
            start = comma + 1;
        }
        channelNames.push_back(channelParam.substr(start));
    }
    // same for keys
    std::string keyParam = cmd.params[1];
    std::vector<std::string> keys;
    {
        size_t start = 0;
        size_t comma;
        while ((comma = keyParam.find(',', start)) != std::string::npos)
        {
            keys.push_back(keyParam.substr(start, comma - start));
            start = comma + 1;
        }
        keys.push_back(keyParam.substr(start));
    }

    // build channel + key vector list after split
    std::vector<std::pair<std::string, std::string>> channels;
    for (size_t i = 0; i < channelNames.size(); i++)
    {
        std::string key = (i < keys.size()) ? keys[i] : "";
        channels.push_back({channelNames[i], key});
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

        // Get or create channel
        if (!server.channelExists(channelName))
            server.createChannel(channelName, clientUser);
        Channel& channel = server.getChannel(channelName);

        // If already in channel, silently skip (irssi behaviour)
        if (channel.hasMember(clientUser.get_ClientUser_fd()))
            continue;

        // ORDER: i k l
        // +i are you invited?              yes/no/not needed
        // +k do you have the password?     yes/no/not needed
        // +l is the room full?             yes/no
        // TODO: CmdInvite
        // TODO: CmdInvite flagcheck (clientUser._isInvited?)
        // check if channel is invite only
        if (channel.isInviteOnly() == true) // && not invited
        {   // ERR_INVITEONLYCHAN (473)
            clientUser.get_outputBuffer().append(
                ":server 473 " + clientUser.getNickname() + " " + channelName +
                " :Cannot join channel (+i)\r\n");
            continue;
        }

        // TODO: add into for loop above for multiple channel checking.
        // check if channel is invite only
        if ((cmd.params[1] != channel.getKey()) && !channel.getKey().empty())
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

        // Build the JOIN prefix: :nick!user@host
        std::string prefix = ":" + clientUser.getNickname() + "!" +
                             clientUser.getUsername() + "@ircserver";

        // Broadcast JOIN to all members (including the joining user)
        std::string joinMsg = prefix + " JOIN " + channelName + "\r\n";
        server.broadcastToChannel(channelName, joinMsg);

        // Send topic (332) or no-topic (331)
        if (!channel.getTopic().empty())
        {
            clientUser.get_outputBuffer().append(
                ":server 332 " + clientUser.getNickname() + " " + channelName +
                " :" + channel.getTopic() + "\r\n");
        }
        else
        {
            clientUser.get_outputBuffer().append(
                ":server 331 " + clientUser.getNickname() + " " + channelName +
                " :No topic is set\r\n");
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
