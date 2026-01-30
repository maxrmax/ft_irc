/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/27 20:55:11 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//Client.cpp
// #include "../../includes/Client.hpp"
#include "../../includes/ircserv.hpp"


Client::~Client()
{
    std::cout << "Client with fd:" << client_fd << " destructed." << std::endl;
    //errno
};

Client::Client():client_fd(-1){};

Client::Client(int filedescriptor):client_fd(filedescriptor){};


void Client::set_client_fd(int filedescriptor)
{
    client_fd = filedescriptor;
};
void Client::set_message_received(std::string string_buffer)
{
    message_received.append(string_buffer);
};
int Client::get_client_fd()
{
    return client_fd;
};
std::string Client::get_message_received()
{
    return(message_received);
};
std::string Client::get_message_put_together()
{
    return(message_put_together);
};

void Client::put_message_together()
{
    message_put_together += message_received;
    // message_received = "";
    message_received.clear();
};
