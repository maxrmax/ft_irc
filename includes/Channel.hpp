/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 14:05:17 by ngoyat            #+#    #+#             */
/*   Updated: 2026/03/10 17:20:29 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_set>

class ClientUser;

/**
 * Channel
 *
 * Represents an IRC channel:
 * - stores name, topic and mode flags (invite-only, topic protection)
 * - maintains member fds, operator fds and invited fds (all are file descriptors, not pointers)
 *
 * Ownership / lifecycle notes:
 * - Channel stores FDs (int) that reference ClientUser instances owned elsewhere (Server / connection manager).
 *   Channel does not own or delete ClientUser objects.
 * - Channel objects are typically stored/owned by Server (e.g. in an unordered_map). Callers that take
 *   references to Channel must not keep them across operations that may rehash/modify the container.
 */
class Channel
{
private:
    std::string                 _name;          // channel name (e.g. "#foo")
    std::string                 _topic;         // channel topic text
    bool                        _topicFlag;     // topic protection mode (+t)
    std::set<int>               _member_fds;    // member file descriptors (ordered set for deterministic iteration)
    std::unordered_set<int>     _operator_fds;  // fds of channel operators (first to join / +o)
    std::unordered_set<int>     _invited_fds;   // fds invited by +I (cleared on successful join)
    bool                        _inviteFlag;    // invite-only mode (+i)
    unsigned int                _userLimit;     // 0 == no limit
    std::string                 _key;           // channel key/password (empty == no key)

public:
    Channel();
    Channel(const std::string &name, ClientUser &founder);
    ~Channel();

    const std::string   &getName() const;

    const std::string   &getTopic() const;
    void                setTopic(const std::string &topic);
    
    void                setTopicFlag(char sign);
    bool                getTopicFlag() const;

    const std::set<int> &getMembers() const;
    void                addMember(int fd);
    void                removeMember(int fd);
    bool                hasMember(int fd) const;

    void                setOperator(int fd);
    void                unsetOperator(int fd);
    bool                isOperator(int fd) const;

    void                setInvited(int fd);
    void                unsetInvited(int fd);
    bool                isInvited(int fd) const;

    void                setInviteOnly(char sign);
    bool                isInviteOnly() const;

    void                setUserLimit(std::string limit);
    unsigned int        getUserLimit() const;

    void                setKey(std::string key);
    std::string         getKey() const;
};
