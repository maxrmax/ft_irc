/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientUserUser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/12 21:00:41 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//ClientUser.cpp
// #include "../../includes/ClientUser.hpp"
// #include "../../includes/OutputBuffer.hpp"
#include "../../../includes/ircserv.hpp"


ClientUser::~ClientUser()
{
    // std::cout << "ClientUser with fd:" << ClientUser_fd << " destructed." << std::endl;
    //errno
};

ClientUser::ClientUser():ClientUser_fd(-1){};

ClientUser::ClientUser(int filedescriptor):ClientUser_fd(filedescriptor){};


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