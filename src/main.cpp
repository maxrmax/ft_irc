/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:10:50 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/15 17:15:11 by nsloniow         ###   ########.fr       */
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
    Server  irc_server;
    if (irc_server.get_server_ready(std::stoi(argv[1])) == -1)
        return -1;
    std::cout << "Server created. fd = " << irc_server.get_server_fd() << std::endl;

    //run server
    return(runServer(irc_server));
    
    
    // //delete what I alloced with new
    // //do inside destructor
    // return (0);
}
