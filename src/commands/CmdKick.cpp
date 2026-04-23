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

#include "CmdKick.hpp"
#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/

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
        // ERR_NEEDMOREPARAMS (461)
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

        if (!channelNameIsValid(channelName) || !server.channelExists(channelName))
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

        ClientUser* target = server.getClientByNick(username);
        if (!target)
        {
            // ERR_NOSUCHNICK 401
            clientUser.get_outputBuffer().append(
                ":server 401 " + clientUser.getNickname() + " " + username +
                " :No such nick\r\n");
            continue;
        }

        if (!channel.hasMember(target->get_ClientUser_fd()))
        {
            // ERR_USERNOTINCHANNEL 441
            clientUser.get_outputBuffer().append(
                ":server 441 " + clientUser.getNickname() + " " + username + " " + channelName +
                " :They aren't on that channel\r\n");
            continue;
        }

        std::string reason = "kicked";
        if (cmd.params.size() > 2 && !cmd.params[2].empty())
            reason = cmd.params[2];

        std::string kickMsg = ":" + clientUser.getNickname() + 
                             "!" + clientUser.getUsername() + 
                             "@ircserver" + 
                             " KICK " + channelName + " " +
                              target->getNickname() + " :" + 
                              reason;

        server.broadcastToChannel(channelName, kickMsg);
        
        channel.removeMember(target->get_ClientUser_fd());

        if (channel.getMembers().empty())
            server.removeChannel(channelName);
    }
}
