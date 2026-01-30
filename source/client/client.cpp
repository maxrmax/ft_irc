/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/23 11:57:02 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/client.hpp"


client::~client()
{
    //close fd for client
    if (client_fd != -1)
    {
        if (close(client_fd) == 0)
        {    
            std::cout << "Client filedescriptor closed for a good nights sleep." << std::endl;
            client_fd = -1;
        }
        else 
        {
            std::cout << "Client filedescriptor could not be closed." << std::endl;
        }
    }       
        
    //errno
};

client::client():client_fd(-1){};

client::client(int filedescriptor):client_fd(filedescriptor){};


void client::set_client_fd(int filedescriptor)
{
    client_fd = filedescriptor;
};

int client::get_client_fd()
{
    return client_fd;
};
