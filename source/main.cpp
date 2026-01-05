/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:10:50 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/05 19:08:11 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ircserv.hpp"

int check_arguments(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "Run like : ./ircserv <port> <connection password>" << std::endl;
        return (-1);
    }
    if (!is_digits_only(argv[1]) || std::stoi(argv[1]) < 1028)
    {
        std::cout << "Run like : ./ircserv <port> <connection password>" << std::endl;
        std::cout << "Port needs to be a number above 1027." << std::endl;
        return (-1);
    }
    return (0);
}


int main(int argc, char **argv)
{
    //are arguments valid
    if (check_arguments(argc, argv) == -1)
        return (-1);
    
    //get server ready
    server  irc_server;
    // if (irc_server.get_server_ready((int)argv[1]) == -1)
    if (irc_server.get_server_ready(std::stoi(argv[1])) == -1)
        return -1;
    std::cout << "Server created. fd =" << irc_server.get_server_fd() << std::endl;

    
    // wait for connections or just sleep
    // sleep(1); // placeholder so the server doesn’t exit
    // int client_fd = accept(irc_server.get_server_fd(), nullptr, nullptr);
    // std::cout << "Client connected! fd=" << client_fd << std::endl;
    //accept() 
    
    //get fd for our client/user and some more stuff
    //init server_address as it is a pointer
    //init socklen as it is a pointer here
    // socklen_t server_address_length = sizeof(server_address);
    socklen_t server_address_length = sizeof(irc_server.get_server_address());
    sockaddr_in server_address_in = irc_server.get_server_address();
    int client_fd = -1;
    // let server run and accept
    while (true)
    {
        client_fd = accept(irc_server.get_server_fd(), (struct sockaddr *)&server_address_in, &server_address_length);
        if ( client_fd < 0)
        {
            std::cout << "Client acception failed." << std::endl;
            return -1;
        }
        std::cout << "Client fd = " << client_fd << std::endl;
    }

    
    //delete what I alloced with new
    //do inside destructor
    
    //no need to double free as destructor is called automatically AGAIN at ending programm
    //irc_server.~server();
    return (0);
}