/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 19:49:40 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/10 17:08:07 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/ircserv.hpp"

Channel::Channel() : _name(""), _topic("") {}

Channel::Channel(const std::string &name, ClientUser &founder)
    : _name(name)
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

// to be used with CmdTopic later
bool Channel::checkTopicFlag()
{
    return _topicFlag;
}

void Channel::setInviteOnly(char sign)
{
    if (sign == '+')
        _inviteFlag = true;
    else if (sign == '-')
        _inviteFlag = false;
}

bool Channel::checkInviteOnly()
{
    return _inviteFlag;
}

// CmdInvite to do flag_check control
void Channel::addMember(int fd)
{
    _member_fds.insert(fd);
}

void Channel::removeMember(int fd)
{
    _member_fds.erase(fd);
}

bool Channel::hasMember(int fd) const
{
    return _member_fds.count(fd) > 0;
}

void Channel::setOperator(int fd)
{
    _operator_fds.insert(fd);
}

bool Channel::isOperator(int fd) const
{

    return _operator_fds.count(fd) > 0;
}

const std::set<int> &Channel::getMembers() const
{
    return _member_fds;
}
