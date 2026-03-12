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

    // IRC allows joining multiple channels: JOIN #a,#b,#c
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

    for (const std::string& channelName : channelNames)
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
        Channel& channel = server.getOrCreateChannel(channelName, clientUser);

        // If already in channel, silently skip (irssi behaviour)
        if (channel.hasMember(clientUser.get_ClientUser_fd()))
            continue;

        // Add member
        channel.addMember(clientUser);

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
