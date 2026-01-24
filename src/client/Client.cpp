/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/24 12:17:52 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//Client.cpp
// #include "../../includes/Client.hpp"
#include "../../includes/ircserv.hpp"


Client::~Client()
{
    //do not close filedescriptors here anymore
    //we create Client object deep inside some nested functions accept_client
    //there we receive the fd and it is opened. 
    //we copy the constructed client into mapping fd to Client object
    //thus those values belong and are owned by the mapping 
    //when we leave accept_client the constructed Client inside there goes out of scope
    //and this very deconstructor is being called and then would close our fd.
    // //close Client fd
    // if (client_fd != -1)
    // {
    //     if (close(client_fd) == 0)
    //     {    
    //         std::cout << "Client filedescriptor closed for a good nights sleep." << std::endl;
    //         client_fd = -1;
    //     }
    //     else 
    //     {
    //         std::cout << "Client filedescriptor could not be closed." << std::endl;
    //     }
    // }       
        
    std::cout << "Client with fd:" << client_fd << " destructed." << std::endl;
    //errno
};

Client::Client():client_fd(-1){};

Client::Client(int filedescriptor):client_fd(filedescriptor){};


void Client::set_client_fd(int filedescriptor)
{
    client_fd = filedescriptor;
};
void Client::set_message(std::string string_buffer)
{
    message.append(string_buffer, sizeof(string_buffer));
};
int Client::get_client_fd()
{
    return client_fd;
};
