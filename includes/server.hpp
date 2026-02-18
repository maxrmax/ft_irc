/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:05:52 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/18 06:00:14 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ircserv.hpp"

#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "commandDispatcher.hpp"

//saver than define a macro is to do a const
//this is in its very own scope for each programm this header is included
//it is not a global variable which would be extern <global variable name>
const int   ADDRESS_FAMILY  =   AF_INET;
const int   PORT_LISTEN     =   6667;  


class Server
{
    private:
        int         server_fd;
        int         server_port;
        //INternet not incomming
        //man 7 ip explains the IPv4 address
        sockaddr_in server_address;

        CommandDispatcher dispatcher;

        //create maping for (unique index) fd to Client object 
        //unordered map jumps to item by index and is faster than (sorted) mappoll_client__mapping_via_fd;
        //int is the index which is equal to client_accepted_fd
        //Client is the type we map to.
        // std::unordered_map<int, ClientUser> poll_clientUser__mapping_via_fd;
        
        std::vector <std::string> nicknames;
        // std::vector <std::string, std::string> nicknames_history;
        std::unordered_map <std::string, std::vector <std::string>> nicknames_history;
        std::unordered_map <std::string, ClientUser*> nick_clientUser;
        
    
    public:
        ~Server();
        Server();
        Server(int filedescriptor, int port);

        int         get_server_fd();
        sockaddr_in get_server_address();
        
        int                 get_server_ready(int port);
        // void                clean_up();

        CommandDispatcher   &get_dispatcher();
        bool                NickIsAlreadyRegistered(std::string nick) const;
        void                Nicknames_storing(std::string nick);
        void                NicknamesHistory_storing(std::string nicknew, std::string nickOld);
        void                Nick_ClientUser_mapping(ClientUser &clientUser);
};