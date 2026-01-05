/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:10:50 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/05 12:44:39 by nsloniow         ###   ########.fr       */
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
    if (!is_digits_only(argv[1]))
    {
        std::cout << "Run like : ./ircserv <port> <connection password>" << std::endl;
        std::cout << "Port needs to be a number." << std::endl;
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



    
    //delete what I alloced with new
    //do inside destructor
    
    //no need to double free as destructor is called automatically AGAIN at ending programm
    //irc_server.~server();
    return (0);
}