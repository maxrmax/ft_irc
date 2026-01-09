/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:47:16 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/09 17:53:42 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/poll.hpp"
#include "../../includes/server.hpp"

int acceptClient(Server &irc_server, std::vector<pollfd> &poll_fd)
{
    //init server_address as it is a pointer
    //init socklen as it is a pointer here
    //socklen_t server_address_length = sizeof(server_address);
    socklen_t server_address_length = sizeof(irc_server.get_server_address());
    sockaddr_in server_address_in = irc_server.get_server_address();

    // int client_fd_accept = -1;
    int client_fd_accept = accept(irc_server.get_server_fd(), (struct sockaddr *)&server_address_in, &server_address_length);
    if ( client_fd_accept < 0)
    {
        // non-blocking needs check on errno so it keeps looping until we finish our loop 
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // No more clients waiting to be accept()ed; return to poll loop
            // accept() would block — no more pending connections; closing accept loop, return to poll loop             
            //break closes the nearest loop               
            //we go back and out of that loop we came from
            //break;
            return -2;
        }
        else
        {
            std::cout << "Client acception failed." << std::endl;
            return -1;
        }
    }
    std::cout << "Client accepted fd = " << client_fd_accept << std::endl;
    // Make the client socket non-blocking
    fcntl(client_fd_accept, F_SETFL, O_NONBLOCK);
    //append client fd to poll list
    pollfd   temp;
    temp.fd         = client_fd_accept;
    temp.events     = POLLIN;
    temp.revents    = 0;
    poll_fd.push_back(temp);
    return 0;
}

int clients_waiting(Server &irc_server, std::vector<pollfd> &poll_fd)
{
    // accept all clients that are to be ready as per poll(fd) desicion
    // int client_fd_accept = -1;
    bool clients_waiting = true;
    // while (true) 
    while (clients_waiting)
    {       
        //create sockets for our client/user, get fd and some more stuff
        switch (acceptClient(irc_server, poll_fd))
        {
        case -1:
            return -1;
        case -2:
            clients_waiting = false;
        }
    }
    return 0;
}

int receive_message(std::vector<pollfd> &poll_fd, int i)
{
    // if (poll_fd[i].revents == POLLIN)
    //Bitmask in revents 0000, POLLIN least bit set, bitwise AND
    //& we check if it is set as well
    if (poll_fd[i].revents & POLLIN)
    {
        char    msg[1024]; 
        int     read_len =0;
        read_len = recv(poll_fd[i].fd,msg, sizeof(msg)-1,0);
        msg[read_len] = '\0';
        std::cout << poll_fd[i].fd << ": \n" << msg << std::endl;
    }
    return 0;
}

int process_ready_fd(Server &irc_server, std::vector<pollfd> &poll_fd, int i)
{
    // std::cout << "poll fd = " << poll_fd[i].fd<< std::endl;
    if (poll_fd[i].fd == irc_server.get_server_fd())
    {
        clients_waiting(irc_server, poll_fd);
    }
    else
    {
        receive_message(poll_fd, i);
    }
    return 0;
}

int runPoll(Server &irc_server, std::vector<pollfd> &poll_fd)
{
        // poll()—Synchronous I/O Multiplexing
        // let the system do the dirty stuff of looping over socket fds and check for data, that got received
        // data() = pointer to first elementS
        int poll__fd_ready__amount = poll(poll_fd.data(), poll_fd.size(), -1);
        //if not fd is ready
        if ( poll__fd_ready__amount < 0)
        {
            // non-blocking needs check on errno so it keeps looping until we finish our loop 
            //return -1 can be either
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                //No client ready right now, just skip and continue loop
                //Skips the rest of the current iteration of the closest loop and starts the next iteration immediately.
                //Jump to end of loop, which now is outside of this function, so instead of continue, we return 0, as all is good 
                // continue;
                return 0;
            }
            else
            {
                std::cout << "Polling failed." << std::endl;
                return -1;
            }
        }
        // std::cout << "Polling " << poll__fd_ready__amount << std::endl;
        
        for (size_t i = 0;i < poll_fd.size(); i++)
        {
            process_ready_fd(irc_server, poll_fd, i);
        }   
    return 0;
}

int runServer(Server &irc_server)
{
    //create array for fd to poll, to check, if ready with data
    std::vector<pollfd> poll_fd;
    //store server socket fd in poll array
    pollfd   temp;
    temp.fd         = irc_server.get_server_fd();
    temp.events     = POLLIN;
    temp.revents    = 0;
    poll_fd.push_back(temp);
    
    // //create sockets for our client/user, get fd and some more stuff
    // //init server_address as it is a pointer
    // //init socklen as it is a pointer here
    // //socklen_t server_address_length = sizeof(server_address);
    // socklen_t server_address_length = sizeof(irc_server.get_server_address());
    // sockaddr_in server_address_in = irc_server.get_server_address();
    // int client_fd_accept = -1;
    // let server run and accept
    while (true)
    {
       if (runPoll(irc_server, poll_fd) == -1)
        return -1;
    }
    return 0;
}
