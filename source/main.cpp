/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:10:50 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/08 18:41:42 by nsloniow         ###   ########.fr       */
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
    if (irc_server.get_server_ready(std::stoi(argv[1])) == -1)
        return -1;
    std::cout << "Server created. fd =" << irc_server.get_server_fd() << std::endl;

    // pollfd  *client_fd = NULL;
    std::vector<pollfd> poll_fd;
    // int poll_fd_cnt = 0;^
    pollfd   temp;
    temp.fd     = irc_server.get_server_fd();
    temp.events = POLLIN;
    poll_fd.push_back(temp);
    // wait for connections or just sleep
    // sleep(1); // placeholder so the server doesn’t exit
    // int client_fd = accept(irc_server.get_server_fd(), nullptr, nullptr);
    // std::cout << "Client connected! fd=" << client_fd << std::endl;
    //accept() 
    
    //create sockets for our client/user, get fd and some more stuff
    //init server_address as it is a pointer
    //init socklen as it is a pointer here
    //socklen_t server_address_length = sizeof(server_address);
    socklen_t server_address_length = sizeof(irc_server.get_server_address());
    sockaddr_in server_address_in = irc_server.get_server_address();
    int client_fd_accept = -1;
    // let server run and accept
    while (true)
    {
  
        // poll()—Synchronous I/O Multiplexing
        // let the system do the dirty stuff of looping over socket fds and check for data, that got received
        // What you really want to be able to do is somehow monitor a bunch of sockets at once and then handle the ones that have data ready. This way you don’t have to continuously poll all those sockets to see which are ready to read.
        // it is slow with gigantic amounts
        // int poll(struct pollfd *fds, nfds_t numberOfFdTypes, int timeout)
        // data() = pointer to firest elementS
        int poll__fd_ready__amount = poll(poll_fd.data(), poll_fd.size(), -1);
        if ( poll__fd_ready__amount < 0)
        {
            std::cout << "Polling failed." << std::endl;
            return -1;
        }
        std::cout << "Polling " << poll__fd_ready__amount << std::endl;
        
        for (size_t i = 0;i < poll_fd.size(); (i++))
        {
        if (poll_fd[i].fd == irc_server.get_server_fd())
            {
                // client_fd = accept(irc_server.get_server_fd(), (struct sockaddr *)&server_address_in, &server_address_length);
                client_fd_accept= accept(irc_server.get_server_fd(), (struct sockaddr *)&server_address_in, &server_address_length);
                if ( client_fd_accept < 0)
                {
                    std::cout << "Client acception failed." << std::endl;
                    return -1;
                }
                std::cout << "Client fd = " << client_fd_accept << std::endl;
                //append client fd to poll list
                // client_fd[client_fd_cnt].fd     = client_fd_accept;
                // client_fd[client_fd_cnt].events = POLLIN;
                temp.fd = client_fd_accept;
                poll_fd.push_back(temp);
            }
            
            std::cout << "poll fd = " << client_fd_accept << std::endl;
        }   
    }

    
    //delete what I alloced with new
    //do inside destructor
    
    //no need to double free as destructor is called automatically AGAIN at ending programm
    // we’re going to ask the operating system to do all the dirty work for us, and just let us know when some data is ready to read on which sockets. In the meantime, our process can go to sleep, saving system resources.
    //irc_server.~server();
    return (0);
}