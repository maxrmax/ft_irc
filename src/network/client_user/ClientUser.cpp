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

#include "ClientUser.hpp" // "inputBuffer.hpp" -> <string> // "OutputBuffer.hpp" -> <string>
#include <iostream>

ClientUser::~ClientUser()
{
    // current debug until quit resolved with netcat
    std::cout << "ClientUser with fd:" << ClientUser_fd << " destructed." << std::endl;
}

ClientUser::ClientUser():ClientUser_fd(-1), registered(false), passAccepted(false), toDisconnect(false)
{
    std::cout << "ClientUser without fd constructed." << std::endl;
};

ClientUser::ClientUser(int filedescriptor):ClientUser_fd(filedescriptor), registered(false), passAccepted(false), toDisconnect(false)
{
    std::cout << "ClientUser with fd:" << ClientUser_fd << " constructed." << std::endl;
};


void ClientUser::set_ClientUser_fd(int filedescriptor)
{
    ClientUser_fd = filedescriptor;
}

int ClientUser::get_ClientUser_fd()
{
    return ClientUser_fd;
}

InputBuffer &ClientUser::get_inputBuffer()
{
    return (inputBuffer);
}

OutputBuffer &ClientUser::get_outputBuffer()
{
    return (outputBuffer);
}

void ClientUser::setNickname(const std::string& nick)
{
    nickname = nick;
}

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

bool ClientUser::hasNick() const
{
    return (!nickname.empty());
}

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

// return 0, when password isn't accepted or nickname or username is empty.
bool ClientUser::isReadyToRegister() const
{
    if (passAccepted == false || nickname.empty() || username.empty())
        return false;
    return true;
}

// Cleanup
// TODO 4
void ClientUser::setToDisconnect(bool disconnect)
{
    toDisconnect = disconnect;
}

bool ClientUser::isToDisconnect() const
{
    return toDisconnect;
}
