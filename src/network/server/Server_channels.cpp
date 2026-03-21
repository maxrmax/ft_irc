/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_channels.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 14:07:08 by ngoyat            #+#    #+#             */
/*   Updated: 2026/03/16 18:09:28 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/

// ── Channel management ────────────────────────────────────────────────────────

std::vector<std::string> Server::getChannelsOfClientFd(int fd)
{
    // goal: get a list of all channels this user is part of
    std::vector<std::string> result;

    for (const auto& entry : _channels)
    {
        const std::string& channelName = entry.first;
        const Channel& channel = entry.second;

        if (channel.hasMember(fd))
            result.push_back(channelName);
    }
    return result;
}

bool Server::channelExists(const std::string& name) const
{
    return _channels.count(name) > 0;
}

Channel& Server::createChannel(const std::string& name, ClientUser& clientUser)
{
    if (_channels.count(name) == 0)
        _channels[name] = Channel(name, clientUser);
    return _channels[name];
}

Channel& Server::getChannel(const std::string& name)
{
    return _channels.at(name);
}

void Server::removeChannel(const std::string& name)
{
    _channels.erase(name);
}

// Broadcast a raw message to ALL members of a channel
void Server::broadcastToChannel(const std::string& channelName, const std::string& msg)
{
    if (_channels.count(channelName) == 0)
        return;

    const std::set<int>& members = _channels[channelName].getMembers();
    for (int fd : members)
    {
        auto it = _clients.find(fd);
        if (it != _clients.end())
            it->second->get_outputBuffer().append(msg);
    }
}

// Broadcast to ALL members EXCEPT one (used for PRIVMSG so sender doesn't echo)
void Server::broadcastToChannelExcept(const std::string& channelName,
                                      const std::string& msg,
                                      int excludeFd)
{
    if (_channels.count(channelName) == 0)
        return;

    const std::set<int>& members = _channels[channelName].getMembers();
    for (int fd : members)
    {
        if (fd == excludeFd)
            continue;
        auto it = _clients.find(fd);
        if (it != _clients.end())
            it->second->get_outputBuffer().append(msg);
    }
}

// Returns space-separated nick list for RPL_NAMREPLY
// '@' prefix for channel operator
std::string Server::getChannelMemberNicks(const std::string& channelName) const
{
    if (_channels.count(channelName) == 0)
        return "";

    const Channel& ch = _channels.at(channelName);
    std::string result;

    for (int fd : ch.getMembers())
    {
        auto it = _clients.find(fd);
        if (it == _clients.end())
            continue;

        if (!result.empty())
            result += " ";

        if (ch.isOperator(fd))
            result += "@";

        result += it->second->getNickname();
    }
    return result;
}

// ── Client lookup ─────────────────────────────────────────────────────────────

ClientUser* Server::getClientByFd(int fd)
{
    auto it = _clients.find(fd);
    if (it == _clients.end())
        return nullptr;
    return it->second;
}

ClientUser* Server::getClientByNick(const std::string& nick)
{
    auto it = nick_clientUser.find(nick);
    if (it == nick_clientUser.end())
        return nullptr;
    return it->second;
}

// Call this when a client connects so broadcastToChannel can find them
void Server::registerClientFd(int fd, ClientUser* client)
{
    _clients[fd] = client;
}

// Call this when a client disconnects
void Server::unregisterClientFd(int fd)
{
    // cleanup of entire client
    auto clientIt = _clients.find(fd);
    if (clientIt != _clients.end())
    {
        std::string nick = clientIt->second->getNickname();
        nick_clientUser.erase(nick);
        nicknames_history[nick] = nullptr;
    }

    std::vector<std::string> channelList = getChannelsOfClientFd(fd);
    for (const auto& channelName : channelList)
    {
        Channel& channel = getChannel(channelName);
        channel.removeMember(fd);
        if (channel.getMembers().empty())
            removeChannel(channelName);
    }
        
    _clients.erase(fd);
}
