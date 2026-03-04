#include "../../../includes/ircserv.hpp"

Channel::Channel() : _operator_fd(-1) {}

Channel::Channel(const std::string& name, ClientUser& founder)
    : _name(name), _operator_fd(founder.get_ClientUser_fd())
{
    _member_fds.insert(founder.get_ClientUser_fd());
}

Channel::~Channel() {}

const std::string& Channel::getName() const { return _name; }
const std::string& Channel::getTopic() const { return _topic; }
void Channel::setTopic(const std::string& topic) { _topic = topic; }

void Channel::addMember(ClientUser& client)
{
    _member_fds.insert(client.get_ClientUser_fd());
}

void Channel::removeMember(int fd)
{
    _member_fds.erase(fd);
}

bool Channel::hasMember(int fd) const
{
    return _member_fds.count(fd) > 0;
}

bool Channel::isOperator(int fd) const
{
    return _operator_fd == fd;
}

const std::set<int>& Channel::getMembers() const
{
    return _member_fds;
}