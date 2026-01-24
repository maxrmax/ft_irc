/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:47:16 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/24 12:42:51 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// runServer.cpp

// #include "../../includes/poll.hpp"
// #include "../../includes/server.hpp"
#include "../../includes/ircserv.hpp"

int acceptClient(Server &irc_server, std::vector<pollfd> &poll_fd, std::unordered_map<int, Client> &poll_client__mapping_via_fd)
{
    //init server_address as it is a pointer
    //init socklen as it is a pointer here
    //socklen_t server_address_length = sizeof(server_address);
    socklen_t   server_address_length = sizeof(irc_server.get_server_address());
    sockaddr_in server_address_in = irc_server.get_server_address();

    // int client_fd_accept = -1;
    int client_accept_fd = accept(irc_server.get_server_fd(), (struct sockaddr *)&server_address_in, &server_address_length);
    if ( client_accept_fd < 0)
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
    std::cout << "Client accepted fd = " << client_accept_fd << std::endl;
    // Make the client socket non-blocking
    fcntl(client_accept_fd, F_SETFL, O_NONBLOCK);
    //append client fd to poll list
    pollfd   temp;
    temp.fd         = client_accept_fd;
    temp.events     = POLLIN;
    temp.revents    = 0;
    poll_fd.push_back(temp);
    
    //create Client object
    Client client_created(client_accept_fd);
    //map fd in pollfd to fd in client_created
    //unordered map jumps to item by index and is faster than (sorted) map
    // The map owns the Client
    // The Client lives inside the map
    poll_client__mapping_via_fd[client_accept_fd] = client_created;
    //we do not create client here
    return 0;
}

int clients_waiting(Server &irc_server, std::vector<pollfd> &poll_fd, std::unordered_map<int, Client> &poll_client__mapping_via_fd)
{
    // accept all clients that are to be ready as per poll(fd) desicion
    // int client_fd_accept = -1;
    bool clients_are_waiting = true;
    // while (true) 
    while (clients_are_waiting)
    {       
        //create sockets for our client/user, get fd and some more stuff
        //accept_client return 0 -> accepted successfully
        switch (acceptClient(irc_server, poll_fd, poll_client__mapping_via_fd))
        {
        case -1:
            return -1; // acception faild
        case -2:
            clients_are_waiting = false; //nomoreclients are waiting right now, so we leave the unlimited loop to not block
        }
    }
    
    return 0;
}

int receive_message(std::vector<pollfd> &poll_fd, int i, std::unordered_map<int, Client> &poll_client__mapping_via_fd)
{
    int polled_fd = poll_fd[i].fd;
    // if (poll_fd[i].revents == POLLIN)
    //Bitmask in revents 0000, POLLIN least bit set, bitwise AND
    //& we check if it is set as well
    // if (poll_fd[i].revents & POLLIN)
    std::cout << __LINE__ << ": \n" << std::endl;
    
    int     read_len =0;
    // if (poll_fd[i].revents & POLLIN)
    // {
    //     std::cout << __LINE__ << ": \n" << std::endl;
    //     char        read_buffer[1024];
    //     int         read_len =0;
    //     read_len =  recv(polled_fd, read_buffer, sizeof(read_buffer), 0);
    //     std::cout << __LINE__ << ": \n" << std::endl;
    //     poll_client__mapping_via_fd[polled_fd].set_message(read_buffer);
    //     std::cout.write(read_buffer, read_len) << std::endl;
    // }
    std::cout << "revents=" << poll_fd[i].revents << " POLLIN=" << POLLIN << std::endl;

                    char    msg[1024]; 
                    read_len = recv(poll_fd[i].fd,msg, sizeof(msg)-1,0);
                    msg[read_len] = '\0';
                    std::cout << poll_fd[i].fd << ": " << msg << std::endl;
    // }
    std::cout << __LINE__ << ": \n" << std::endl;
    if (read_len > 0)
    {
        msg[read_len] = '\0';
        std::cout << "Received: " << msg << std::endl;
    }
    else if (read_len == 0)
    {
        std::cout << "Client closed connection" << std::endl;
        // remove fd from poll list if you want
    }
    else // read_len < 0
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // no data available right now, keep looping
        }
        else
        {
            perror("recv");
            // handle error
        }
    }
    return 0;
}

int process_ready_fd(Server &irc_server, std::vector<pollfd> &poll_fd, int i, std::unordered_map<int, Client> &poll_client__mapping_via_fd)
{
    // std::cout << "poll fd = " << poll_fd[i].fd<< std::endl;
    if (poll_fd[i].fd == irc_server.get_server_fd())
    {
        clients_waiting(irc_server, poll_fd, poll_client__mapping_via_fd);
    }
    else
    {
        if (receive_message(poll_fd, i, poll_client__mapping_via_fd) < 0)
            return -1;
    }
    return 0;
}

int runPoll(Server &irc_server, std::vector<pollfd> &poll_fd, std::unordered_map<int, Client> &poll_client__mapping_via_fd)
{
        // poll()—Synchronous I/O Multiplexing
        // let the system do the dirty stuff of looping over socket fds and check for data, that got received
        // data() = pointer to first elementS
        int poll__fd_ready__amount = poll(poll_fd.data(), poll_fd.size(), -1);
        //if not fd is ready
        if ( poll__fd_ready__amount < 0)
        {
            // non-blocking needs check on errno so it keeps looping until we finish our loop 
            //return -1 can be eitherstd::unordered_map<int, Client> &poll_client__mapping_via_fd
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                //No client ready right now, just skip and continue loop
                //Skips the rest of the current iteration of the clcloseosest loop and starts the next iteration immediately.
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
            if (poll_fd[i].revents & POLLIN)
            {
                std::cout << "Poll fd ready: "  << poll_fd[i].fd << std::endl;
                if (process_ready_fd(irc_server, poll_fd, i, poll_client__mapping_via_fd) < 0)
                {
                    return -1;
                }
                poll_fd[i].revents = 0;
            }
        }   
    return 0;
}
void clean_up(std::unordered_map<int, Client> &poll_client__mapping_via_fd)
{
    //auto &pair figures out the type automatically
    // for (auto &pair : poll_client__mapping_via_fd)
    for (auto &[fd, client] : poll_client__mapping_via_fd)
    {
        // Client destructor will close fd
        // or explicitly close here if you remove it from destructor
        if (fd != -1)
        {
            if (close(fd) == 0)
            {    
                std::cout << "Client filedescriptor closed for a good nights sleep." << std::endl;
            }
            else 
            {
                std::cout << "Client filedescriptor could not be closed." << std::endl;
            }
        }       
    }
    poll_client__mapping_via_fd.clear(); // triggers destructors
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

    //create maping for (unique index) fd to Client object 
    //unordered map jumps to item by index and is faster than (sorted) mappoll_client__mapping_via_fd;
    //int is the index which is equal to client_accepted_fd
    //Client is the type we map to.
    std::unordered_map<int, Client> poll_client__mapping_via_fd;
    
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
       if (runPoll(irc_server, poll_fd, poll_client__mapping_via_fd) == -1)
        // return -1;
            break;
    }
    clean_up(poll_client__mapping_via_fd);
    return 0;
}
