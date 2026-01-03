/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/03 21:40:10 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/server.hpp"


server::~server(){};
server::server():server_fd(-1){};
server::server(int filedescriptor):server_fd(filedescriptor){};


int server::get_server_fd()
{
    return server_fd;
};

int server::get_server_ready()
{
    //man socket
    //cat /etc/protocols -> TCP
    //filedescriptor = socket(IPv4, in stream for TCP, protocol # for TCP)
    server_fd = socket(AF_INET, SOCK_STREAM, 6);

    if (server_fd == -1)
    {
        std::cout << "Server could not be created. Try again." << std::endl;
        return -1;
    }
    //open
    //bind()?

    //connect()

    //close() at end of programm
    return 0;
}