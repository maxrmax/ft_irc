/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/09 09:53:52 by nsloniow         ###   ########.fr       */
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

sockaddr_in server::get_server_address()
{
    return server_address;
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
    
    // just sits there until a packet arrives.  it is blocking
    // Manipulating fd behavior, set flag to nonblocking, so it does not sit there open and blocks the system till something arrives at that socket
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    //setting address informatiom
    std::memset(&server_address, 0, sizeof(server_address));    //setting memory to 0 to not have garbage
    server_address.sin_addr.s_addr  = INADDR_ANY;               //liste on all interfaces
    server_address.sin_family       = ADDRESS_FAMILY;
    // htons converts the unsigned short integer hostshort from host byte order to network byte order.
    // server_address.sin_port         = htons(PORT_LISTEN);
    server_address.sin_port         = htons(server_port);

    //bind
    // if (bind(server_fd, &server_address.sin_addr, sizeof(server_address))  == -1)
    if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address))  == -1)
    {
        std::cout << "Server could not be 'binded'. Try another port." << std::endl;
        return -1;
    }

    // listen
    //backlog is the queu for the kernel 0 - 128 mostly. It will be caped by kernel if lower
    //bottleneck is accept()
    if (listen(server_fd, 1) < 0)
    {
        std::cout << "Listen failed." << std::endl;
        return -1;
    }
    std::cout << "Listening on port " << server_port << "." << std::endl;
    //accept() 
    //get fd for our client/user and some more stuff
    //init socklen as it is a pointer here
    // socklen_t server_address_length = sizeof(server_address);
    // int client_fd = -1;
    // client_fd = accept(server_fd, (struct sockaddr *)&server_address, &server_address_length);
    // if ( client_fd < 0)
    // {
    //     std::cout << "Client acception failed." << std::endl;
    //     return -1;
    // }
    // std::cout << "Client fd = " << client_fd << std::endl;
    

    return 0;
}