/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/18 05:57:22 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "../../../includes/server.hpp"
#include "../../../includes/ircserv.hpp"


Server::~Server()
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

Server::Server():server_fd(-1), server_port(-1){};

Server::Server(int filedescriptor, int port):server_fd(filedescriptor), server_port(port){};

int Server::get_server_fd()
{
    return server_fd;
};

sockaddr_in Server::get_server_address()
{
    return server_address;
};

int Server::get_server_ready(int port)
{
    //cat /etc/protocols -> TCP
    //fd = socket(IPv4, in stream for TCP, protocol # for TCP)
    server_fd   = socket(ADDRESS_FAMILY, SOCK_STREAM, 6);
    if (server_fd == -1)
    {
        std::cout << "Server could not be created. Try again." << std::endl;
        return -1;
    }
    // just sits there until a packet arrives. it is blocking. The call - recv() or accept() waits until it can do something, it waits for clients to accept or data to receive.
    //only when this happenened, the code continues to execute.
    //Doing fork() and having a thread per fd would not need blocking, as it just sits and waits for its own purpose.
    // Manipulating fd behavior, set flag to nonblocking, so it does not sit there open and blocks the system till something arrives at that socket
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    
    // server_port = PORT_LISTEN;
    server_port = port;
    //setting address informatiom
    std::memset(&server_address, 0, sizeof(server_address));    //setting memory to 0 to not have garbage
    server_address.sin_addr.s_addr  = INADDR_ANY;               //liste on all interfaces, Accept connections on all IPv4 addresses of this machine.
    server_address.sin_family       = ADDRESS_FAMILY;
    // htons converts the unsigned short integer hostshort from host byte order to network byte order.
    server_address.sin_port         = htons(server_port);

    //bind fd of our socket of our server to an attached to an IP/port combination
    //ANY address on server_port we put into server_address struct.
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
    return 0;
}

// void Server::clean_up()
// {
//     //auto &pair figures out the type automatically
//     // for (auto &pair : poll_client__mapping_via_fd)
//     for (auto &[fd, client] : poll_clientUser__mapping_via_fd)
//     {
//         // Client destructor will close fd
//         // or explicitly close here if you remove it from destructor
//         if (fd != -1)
//         {
//             if (close(fd) == 0)
//             {    
//                 std::cout << "Client filedescriptor closed for a good nights sleep." << std::endl;
//             }
//             else 
//             {
//                 std::cout << "Client filedescriptor could not be closed." << std::endl;
//             }
//         }       
//     }
//     poll_clientUser__mapping_via_fd.clear(); // triggers destructors
// }

CommandDispatcher &Server::get_dispatcher()
{
    return (dispatcher);
};

bool Server::NickIsAlreadyRegistered(std::string nick) const
{
    if (std::find(nicknames.begin(), nicknames.end(), nick) != nicknames.end())
        return true;
    return false;
}

void Server::Nicknames_storing(std::string nick)
{
    nicknames.push_back(nick);
};

void Server::NicknamesHistory_storing(std::string nickNew, std::string nickOld)
{
    nicknames_history[nickNew].push_back(nickOld);
};

void Server::Nick_ClientUser_mapping(ClientUser &clientUser)
{
    nick_clientUser[clientUser.getNickname()] = &clientUser;
};
