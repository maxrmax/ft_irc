/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/17 15:18:34 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//server.cpp

// #include <algorithm>
// #include "../../../includes/server.hpp"
#include "../../../includes/ircserv.hpp"


Server::~Server()
{
    //close fd for server
    if (server_fd != -1)
    {
        if (close(server_fd) == 0)
        {    
            // std::cout << "Server filedescriptor closed for a good nights sleep. Have some rest, too. Bye. Bye." << std::endl;
            std::cout << "Server filedescriptor closed. Server shut down." << std::endl;
            server_fd = -1;
        }
        else 
        {
            // std::cout << "Server filedescriptor could not be closed. But you do not need to lose sleep over it. I can handle it gracefully." << std::endl;
            std::cout << "Server filedescriptor could not be closed. Operating System will handle this." << std::endl;
        }
    }       
        
    //errno
};

Server::Server():server_fd(-1), server_port(-1), server_password(""){};

Server::Server(int filedescriptor, int port, std::string password):server_fd(filedescriptor), server_port(port), server_password(password){};
int Server::get_server_fd()
{
    return server_fd;
};
std::string Server::get_server_password()
{
    return server_password;
};
sockaddr_in Server::get_server_address()
{
    return server_address;
};
std::vector<pollfd> &Server::getPollFD()
{
    return poll_fd; 
}
const std::vector<pollfd> &Server::getPollFD() const
{
    return poll_fd;
}

int Server::get_server_ready(int port, std::string password)
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
    server_password = password;
    //setting address informatiom
    std::memset(&server_address, 0, sizeof(server_address));    //setting memory to 0 to not have garbage
    server_address.sin_addr.s_addr  = INADDR_ANY;               //listen on all interfaces, Accept connections on all IPv4 addresses of this machine.
    server_address.sin_family       = ADDRESS_FAMILY;
    // htons converts the unsigned short integer hostshort from host byte order to network byte order.
    server_address.sin_port         = htons(server_port);
    

    // set socket options so we can reuse a still bound port
    // On main exit, server instance goes out of scope and server is called and closes server socket fd. This is clean
    // However, TCP being a bit on the cautiouse side, hoggs port with TIME_WAIT for a while - ~2min, just in case some latecomer packages arrive. But we do not care about this
    // intrusive guardiance of tcp. Because we do not exist anymore to handle it. Our server is closed, dead.
    // With setsockopt we tell the operating system, we use this port anyways.
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (void *)1, 1) == -1)
    int opt_val = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val)) == -1)
    {
        std::cout << "Server could not be 'binded'. Try another port." << std::endl;
        return -1;
    }
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
    if (listen(server_fd, 1) < 0) // might want to change to SOMAXCONN or ~10 later to not drop connection attempts
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
    // could be without "> 0" as count returns 0 or 1 in unordered_map anyway
    // this is just better practice
    return nick_clientUser.count(nick) > 0;
    // old vector way
    // if (std::find(nicknames.begin(), nicknames.end(), nick) != nicknames.end())
        // return true;
    // return false;
}

// deprecated because of vector replacement
// void Server::Nicknames_storing(std::string nick)
// {
//     if (!NickIsAlreadyRegistered(nick))
//         nicknames.push_back(nick);
// };

// void Server::Nicknames_storing(std::string nick)
// {
//     //vector into set as it is easier to erase by string
//     //so push_back no work anymore
//     nicknames.insert(nick);
// };

// void Server::NicknameUnregister(std::string nick)
// {
//     nicknames.erase(nick);
// };

void Server::NicknamesHistory_storing(std::string previouseNickname, ClientUser &clientUser)
{
    nicknames_history[previouseNickname] = &clientUser;
    //nicknames_history[nickNew].push_back(nickOld);
};

void Server::Nick_ClientUser_mapping(ClientUser &clientUser)
{
    nick_clientUser[clientUser.getNickname()] = &clientUser;
};

void Server::printRegisteredNicks()
{
    std::cout << "Registered nicknames: ";
    /* loooook at this, it's the difference of C++98 and C++17
     *
     * the old one we had with the vector
     * for (std::vector<std::string>::const_iterator it = nicknames.begin(); it != nicknames.end(); ++it)
     *
     * C++98
     * for (std::unordered_map<std::string, ClientUser*>::const_iterator it = nick_clientUser.begin();
     *
     * and the C++11 and C++17 simplification
     * const (non modifiable), auto& (reference - not a copy)
     * binds nick to pair.first, client to pair.second
     * ": <container>" to iterate
     * could do [nick, _] by convention but its not enforced
     */
    for (const auto& [nick, client] : nick_clientUser)
        std::cout << nick << " ";
    std::cout << std::endl;
}
