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

class Channel
{
private:
    std::string                 _name;          // channel name
    std::string                 _topic;         // channel topic
    std::set<int>               _member_fds;    // fds of all members
    std::unordered_set<int>     _operator_fds;  // fds of channel operator (first to join/+o)

public:
    Channel();
    Channel(const std::string &name, ClientUser &founder);
    ~Channel();

    const std::string   &getName() const;
    const std::string   &getTopic() const;
    void                setTopic(const std::string& topic);

    void                addMember(ClientUser& client);
    void                removeMember(int fd);
    bool                hasMember(int fd) const;
    void                setOperator(int fd);
    bool                isOperator(int fd) const;
    

    const std::set<int> &getMembers() const;
    // isn't getMembers() used to get the entire list?
    // below can be deleted?
    // Returns comma-separated nick list for RPL_NAMREPLY
    // consider passing clientUser* lookup function instead
    // or resolving nicks at the server level before calling getMemberList
    // std::string         getMemberList(const std::unordered_map<int, ClientUser>& clients) const;
};
