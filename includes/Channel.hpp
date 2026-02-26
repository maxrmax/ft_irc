#pragma once

#include <string>
#include <vector>
#include <set>

class ClientUser;

class Channel
{
private:
    std::string             _name;
    std::string             _topic;
    std::set<int>           _member_fds;    // fds of all members
    int                     _operator_fd;   // fd of channel operator (first to join)

public:
    Channel();
    Channel(const std::string& name, ClientUser& founder);
    ~Channel();

    const std::string&  getName() const;
    const std::string&  getTopic() const;
    void                setTopic(const std::string& topic);

    void                addMember(ClientUser& client);
    void                removeMember(int fd);
    bool                hasMember(int fd) const;
    bool                isOperator(int fd) const;

    const std::set<int>& getMembers() const;

    // Returns comma-separated nick list for RPL_NAMREPLY
    std::string         getMemberList(const std::unordered_map<int, ClientUser>& clients) const;
};
