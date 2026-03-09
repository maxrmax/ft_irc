/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientUser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/09 21:00:01 by mring            ###   ########.fr       */
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

// void ClientUser::registerNick(Server &server)
// {
//       // Check if ready to register after setting username and realname
//     // if (clientUser.isReadyToRegister() &&
//     //     !clientUser.getNickname().empty() &&
//     //     !clientUser.getUsername().empty() &&
//     //     !clientUser.isRegistered())
//     if (isReadyToRegister() && !isRegistered())
//     {
// 		// Register the client with the server
//         // std::cout << __FILE__ << __LINE__ << "setRegistered before" << clientUser.isRegistered() << std::endl;
//         setRegistered(true);
//         server.Nick_ClientUser_mapping(clientUser);
//         // std::cout << __FILE__ << __LINE__ << "setRegistered after " << clientUser.isRegistered() << std::endl;

//         clientUser.get_outputBuffer().append(
//             ":server 001 " + clientUser.getNickname() +
//             " :Welcome to ircserver" +
//             clientUser.getNickname() + "!" +
//             clientUser.getUsername() + "@ircserver\r\n");
//     }
//     server.printRegisteredNicks();
// }