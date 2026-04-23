/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 19:49:40 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/16 14:19:01 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp" // <string> <vector> <set> <unordered_set>
#include "ClientUser.hpp" // "inputBuffer.hpp" -> <string> // "OutputBuffer.hpp" -> <string>

Channel::Channel() : _name(""), _topic(""), _topicFlag(false), _inviteFlag(false), _userLimit(0), _key("") {}

Channel::Channel(const std::string &name, ClientUser &founder)
    : _name(name), _topic(""), _topicFlag(false), _inviteFlag(false), _userLimit(0), _key("") 
{
    _member_fds.insert(founder.get_ClientUser_fd());
    _operator_fds.insert(founder.get_ClientUser_fd());
}

Channel::~Channel() {}

const std::string &Channel::getName() const
{
    return _name;
}

const std::string &Channel::getTopic() const
{
    return _topic;
}

void Channel::setTopic(const std::string &topic)
{
    _topic = topic;
}

void Channel::setTopicFlag(char sign)
{
    if (sign == '+')
        _topicFlag = true;
    else if (sign == '-')
        _topicFlag = false;
}

bool Channel::getTopicFlag() const
{
    return _topicFlag;
}

const std::set<int> &Channel::getMembers() const
{
    return _member_fds;
}

void Channel::addMember(int fd)
{
    _member_fds.insert(fd);
}

void Channel::removeMember(int fd)
{
    _member_fds.erase(fd);
    if (isOperator(fd))
        unsetOperator(fd);
}

bool Channel::hasMember(int fd) const
{
    return _member_fds.count(fd) > 0;
}

void Channel::setOperator(int fd)
{
    _operator_fds.insert(fd);
}

void Channel::unsetOperator(int fd)
{
    _operator_fds.erase(fd);
}

bool Channel::isOperator(int fd) const
{
    return _operator_fds.count(fd) > 0;
}

void Channel::setInvited(int fd)
{
    _invited_fds.insert(fd);
}

void Channel::unsetInvited(int fd)
{
    _invited_fds.erase(fd);
}

bool Channel::isInvited(int fd) const
{
    return _invited_fds.count(fd) > 0;
}

void Channel::setInviteOnly(char sign)
{
    if (sign == '+')
        _inviteFlag = true;
    else if (sign == '-')
        _inviteFlag = false;
}

bool Channel::isInviteOnly() const
{
    return _inviteFlag;
}

void Channel::setUserLimit(std::string limit)
{
    if (limit.empty())
        _userLimit = 0; // 0 for unlimited?
    else
        _userLimit = static_cast<unsigned int>(std::stoi(limit));
}

unsigned int Channel::getUserLimit() const
{
    return _userLimit;
}

void Channel::setKey(std::string key)
{
    _key = key;
}

std::string Channel::getKey() const
{
    return _key;
}