/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:10:50 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/03 21:40:35 by nsloniow         ###   ########.fr       */
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
    if (check_arguments(argc, argv) == -1)
        return (-1);
    
    server  irc_server;
    if (irc_server.get_server_ready() == -1)
        return -1;
    std::cout << "Server created. fd =" << irc_server.get_server_fd() << std::endl;

    return (0);
}