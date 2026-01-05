/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/05 12:36:13 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/server.hpp"


server::~server()
{
    //close fd for server
    if (server_fd != -1)
    {
        if (close(server_fd) == 0)
        {    
            std::cout << "Server filedescriptor closed for a good nights sleep. Have some rest, too. Bye. Bye." << std::endl;
            server_fd = -1;
        }
        else 
        {
            std::cout << "Server filedescriptor could not be closed. But you do not need to lose sleep over it. I can handle it gracefully." << std::endl;
        }
    }       
        
    //errno
};

server::server():server_fd(-1), server_port(-1){};

server::server(int filedescriptor, int port):server_fd(filedescriptor), server_port(port){};

int server::get_server_fd()
{
    return server_fd;
};

int server::get_server_ready(int port)
{
    //open filedescrip        std::cout << "Server could not be created. Try again." << std::endl;tor
    //man socket
    //cat /etc/protocols -> TCP
    //fd = socket(IPv4, in stream for TCP, protocol # for TCP)
    // server_fd = socket(AF_INET, SOCK_STREAM, 6);
    server_fd   = socket(ADDRESS_FAMILY, SOCK_STREAM, 6);
    server_port = port;
    
    if (server_fd == -1)
    {
        std::cout << "Server could not be created. Try again." << std::endl;
        return -1;
    }

    //setting address informatiom
    std::memset(&server_address, 0, sizeof(server_address));  //setting memory to 0 to not have garbage
    server_address.sin_addr.s_addr  = INADDR_ANY;           //just give any addy that is available
    server_address.sin_family       = ADDRESS_FAMILY;
    // server_address.sin_port         = PORT_LISTEN;
    server_address.sin_port         = server_port;

    //bind
    // if (bind(server_fd, ))

    //connect()
    return 0;
}