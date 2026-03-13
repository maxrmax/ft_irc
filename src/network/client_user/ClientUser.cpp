/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientUser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/13 11:33:11 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//ClientUser.cpp

#include "../../../includes/ircserv.hpp"

ClientUser::~ClientUser()
{
    // std::cout << "ClientUser with fd:" << ClientUser_fd << " destructed." << std::endl;
    //errno
};

ClientUser::ClientUser():ClientUser_fd(-1), registered(false), passAccepted(false), toDisconnect(false){};

ClientUser::ClientUser(int filedescriptor):ClientUser_fd(filedescriptor), registered(false), passAccepted(false), toDisconnect(false){};


void ClientUser::set_ClientUser_fd(int filedescriptor)
{
    ClientUser_fd = filedescriptor;
};

int ClientUser::get_ClientUser_fd()
{
    return ClientUser_fd;
};

InputBuffer &ClientUser::get_inputBuffer()
{
    return (inputBuffer);
};

OutputBuffer &ClientUser::get_outputBuffer()
{
    return (outputBuffer);
};

// Identity
void ClientUser::setNickname(const std::string& nick)
{
    nickname = nick;
};

std::string ClientUser::getNickname() const
{
    return nickname;
}

void ClientUser::setUsername(const std::string& user)
{
    username = user;
}

std::string ClientUser::getUsername() const
{
    return username;
}

void ClientUser::setRealname(const std::string& real)
{
    realname = real;
}

std::string ClientUser::getRealname() const
{
    return realname;
}

void ClientUser::setIp(const std::string& ip_str)
{
    ip = ip_str;
}

std::string ClientUser::getIp() const
{
    return ip;
}

// nick check?
bool ClientUser::hasNick() const
{
    return (!nickname.empty());
};


// Auth state
void ClientUser::setPassAccepted(bool accepted)
{
    passAccepted = accepted;
}

bool ClientUser::isPassAccepted() const
{
    return passAccepted;
}

void ClientUser::setRegistered(bool reg)
{
    registered = reg;
}

bool ClientUser::isRegistered() const
{
    return registered;
}

// return 0, when password isn't accepted,
// nickname or username is empty.
bool ClientUser::isReadyToRegister() const
{
    if (passAccepted == false || nickname.empty() || username.empty())
        return false;
    return true;
}

// Cleanup
void ClientUser::setToDisconnect(bool disconnect)
{
    // WIP TODO
    // proper disconnection logic
    toDisconnect = disconnect;
}

bool ClientUser::isToDisconnect() const
{
    // WIP TODO
    return toDisconnect;
}
