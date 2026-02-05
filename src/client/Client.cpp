/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/05 14:12:42 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//Client.cpp
// #include "../../includes/Client.hpp"
// #include "../../includes/OutputBuffer.hpp"
#include "../../includes/ircserv.hpp"


Client::~Client()
{
    // std::cout << "Client with fd:" << client_fd << " destructed." << std::endl;
    //errno
};

Client::Client():client_fd(-1){};

Client::Client(int filedescriptor):client_fd(filedescriptor){};


void Client::set_client_fd(int filedescriptor)
{
    client_fd = filedescriptor;
};

int Client::get_client_fd()
{
    return client_fd;
};
InputBuffer &Client::get_inputBuffer()
{
    return (inputBuffer);
};
OutputBuffer &Client::get_outputBuffer()
{
    return (outputBuffer);
};