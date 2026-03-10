/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientUser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/09 17:16:35 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//ClientUser.cpp

#include "../../../includes/ircserv.hpp"

ClientUser::~ClientUser()
{
    // std::cout << "ClientUser with fd:" << ClientUser_fd << " destructed." << std::endl;
    //errno
};

ClientUser::ClientUser():ClientUser_fd(-1), registered(0){};

ClientUser::ClientUser(int filedescriptor):ClientUser_fd(filedescriptor), registered(0){};


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

void ClientUser::setUsername(const std::string& user)
{
    username = user;
}
void ClientUser::setRealname(const std::string& real)
{
    realname = real;
}

// void client::getNickname(std::string& nick) const
// {
//     nick = nickname;
// }

// void client::getUsername(std::string& user) const
// {
//     user = username;
// }

std::string ClientUser::getNickname() const
{
    return nickname;
}

std::string ClientUser::getUsername() const
{
    return username;
}

// std::string ClientUser::getRealname() const
// {
//     return realname;
// }

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

bool ClientUser::isReadyToRegister() const
{
    if (passAccepted == false)
        return false;
    if (nickname.empty() || username.empty())
        return false;
    return true;
}
