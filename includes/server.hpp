/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:05:52 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/03 21:39:35 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ircserv.hpp"

#include <sys/types.h>
#include <sys/socket.h>


class server
{
    private:
        int server_fd;
    
    public:
        ~server();
        server();
        server(int filedescriptor);

        int get_server_fd();
        
        int get_server_ready();
};