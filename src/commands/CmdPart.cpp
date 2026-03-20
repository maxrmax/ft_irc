/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPart.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 13:17:43 by mring             #+#    #+#             */
/*   Updated: 2026/03/16 18:02:34 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ircserv.hpp"

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

void CmdPart::execute(Server &server, ClientUser &clientUser, const ParsedCommand &cmd)
{
    // PART(cmd) #ch,#ch2,#ch3 (param[0])
    if (cmd.params.empty())
    {
        // ERR_NEEDMOREPARAMS (461)
        clientUser.get_outputBuffer().append(
            ":server 461 " + clientUser.getNickname() + " PART :Not enough parameters\r\n");
        return;
    }
    
    // get comma seperated channel "array"
    std::vector<std::string> channels = split_elements(cmd.params[0]);

    for (const auto& channelName : channels)
    {
        if (!channelNameIsValid(channelName) || !server.channelExists(channelName))
        {
            // ERR_NOSUCHCHANNEL (403)
            clientUser.get_outputBuffer().append(
                ":server 403 " + clientUser.getNickname() + " " + channelName +
                " :No such channel (invalid name)\r\n");
            continue;
        }

        Channel& channel = server.getChannel(channelName);

        // channelmember check
        if (!channel.hasMember(clientUser.get_ClientUser_fd()))
        {
            // ERR_NOTONCHANNEL 442
            clientUser.get_outputBuffer().append(
                ":server 442 " + clientUser.getNickname() + " " + channelName +
                " :You're not on that channel\r\n");
            continue;
        }
        
        // Send PART to all channel members (including the parting user)
        std::string partMsg = ":" + clientUser.getNickname() + 
                              "!" + clientUser.getUsername() +
                              "@ircserver" + " PART " + channelName + "\r\n";
        server.broadcastToChannel(channelName, partMsg);
        

        channel.removeMember(clientUser.get_ClientUser_fd());

        if (channel.getMembers().empty())
            server.removeChannel(channelName);
    }
}

/*
(PART message is broadcast)
403 ERR_NOSUCHCHANNEL
442 ERR_NOTONCHANNEL
461 ERR_NEEDMOREPARAMS
*/

/*
4.2.2 Part message

      Command: PART
   Parameters: <channel>{,<channel>}

   The PART message causes the client sending the message to be removed
   from the list of active users for all given channels listed in the
   parameter string.

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
           ERR_NOTONCHANNEL

   Examples:

   PART #twilight_zone             ; leave channel "#twilight_zone"

   PART #oz-ops,&group5            ; leave both channels "&group5" and
                                   "#oz-ops".
*/