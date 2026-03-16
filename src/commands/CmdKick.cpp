/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdKick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 11:48:54 by mring             #+#    #+#             */
/*   Updated: 2026/03/16 12:08:09 by mring            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

//CmdKick.cpp
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

void CmdKick::execute(Server &server, ClientUser &clientUser, const ParsedCommand &cmd)
{
    // KICK(cmd) #ch(param[0]) user(param[1]) 
    if (cmd.params.empty() || cmd.params.size() < 2)
    {
        clientUser.get_outputBuffer().append(
            ":server 461 " + clientUser.getNickname() + " KICK :Not enough parameters\r\n");
        return;
    }

    std::vector<std::string> channelNames = split_elements(cmd.params[0]);
    std::vector<std::string> users = std::vector<std::string>();
    if (cmd.params.size() > 1 && !cmd.params[1].empty())
        users = split_elements(cmd.params[1]);

    // build channel + key vector list after split
    std::vector<std::pair<std::string, std::string>> channels;
    for (size_t i = 0; i < channelNames.size(); i++)
    {
        std::string username = (i < users.size()) ? users[i] : "";
        channels.push_back({channelNames[i], username});
    }

    for (const auto &[channelName, username] : channels)
    {
        if (!channelNameIsValid(channelName))
        {
            // ERR_NOSUCHCHANNEL (403) reused for invalid name here
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

        // opcheck
        if (!channel.isOperator(clientUser.get_ClientUser_fd()))
        {
            // ERR_CHANOPRIVSNEEDED 482
            clientUser.get_outputBuffer().append(
                ":server 482 " + clientUser.getNickname() + " " + channelName +
                " :You're not channel operator\r\n");
            continue;
        }

        // TODO
        // actual force part lol

    }
}
