/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:05:52 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/05 12:44:36 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// #include "ircserv.hpp"

#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

//saver than define a macro is to do a const
//this is in its very own scope for each programm this header is included
//it is not a global variable which would be extern <global variable name>
const int   ADDRESS_FAMILY  =   AF_INET;
const int   PORT_LISTEN     =   6667;  


class server
{
    private:
        int         server_fd;
        int         server_port;
        sockaddr_in server_address; //INternet not incomming
    
    public:
        ~server();
        server();
        server(int filedescriptor, int port);

        int get_server_fd();
        
        int get_server_ready(int port);
};