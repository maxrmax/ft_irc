/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:47:16 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/18 06:01:40 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// runServer.cpp

// #include "../../includes/poll.hpp"
// #include "../../includes/server.hpp"
#include "../../../includes/ircserv.hpp"


//simple first send message
static int sendMsg(ClientUser &clientUser)
{
    //recv() pull bytes from kernel
    //send() push bytes to kernel
    
    if (clientUser.get_outputBuffer().get_buffer().length() > 0)
    {
        // std::cout << "out BEFORE send: " << clientUser.get_outputBuffer().get_buffer() << std::endl << std::endl;
        ssize_t send_len = send(clientUser.get_ClientUser_fd(), clientUser.get_outputBuffer().get_buffer().c_str(), clientUser.get_outputBuffer().get_buffer().size(), 0);
        if (send_len < 0)
        {
            if (errno == EAGAIN)
            {
                std::cout << "Socket fd not ready to send yet.\n";
            }
            else
            {
                std::cerr << "Error sending to client.\n";
            }
        }
        else
        {
            clientUser.get_outputBuffer().popLine();
        }
        // std::cout << "out after send: " << clientUser.get_outputBuffer().get_buffer() << std::endl;
    }
    return 0;
}

int acceptClientUser(Server &irc_server, std::vector<pollfd> &poll_fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
{
    //create sockets for our client/user, get fd and some more stuff
    // init server_address as it is a pointer
    // init socklen as it is a pointer here
    // socklen_t server_address_length = sizeof(server_address);
    // socklen_t server_address_length = sizeof(irc_server.get_server_address());
    socklen_t   server_address_length = sizeof(irc_server.get_server_address());
    sockaddr_in server_address_in = irc_server.get_server_address();
    int client_accept_fd = accept(irc_server.get_server_fd(), (struct sockaddr *)&server_address_in, &server_address_length);

    if ( client_accept_fd < 0)
    {
        // non-blocking needs check on errno so it keeps looping until we finish our loop 
        if (errno == EAGAIN)
        {
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
    temp.events     = POLLIN | POLLOUT;
    temp.revents    = 0;
    poll_fd.push_back(temp);
    
    //create Client object
    ClientUser client_created(client_accept_fd);
    poll_clientUser__mapping_via_fd[client_accept_fd] = client_created;
    irc_server.registerClientFd(client_accept_fd, &poll_clientUser__mapping_via_fd[client_accept_fd]);
    //we do not create client here

    return 0;
}

int clientUsers_waiting(Server &irc_server, std::vector<pollfd> &poll_fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
{
    // accept all clients that are to be ready as per poll(fd) desicion
    bool clientUsers_are_waiting = true;
    while (clientUsers_are_waiting)
    {       
        //create sockets for our client/user, get fd and some more stuff
        //accept_client return 0 -> accepted successfully
        switch (acceptClientUser(irc_server, poll_fd, poll_clientUser__mapping_via_fd))
        {
        case -1:
            return -1; // acception faild
        case -2:
            clientUsers_are_waiting = false; //no more clients are waiting right now, so we leave the unlimited loop to not block
        }
    }
    
    return 0;
}


int receive_message(Server &irc_server, std::vector<pollfd> &poll_fd, int fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
{
    // int polled_fd = poll_fd[fd].fd;

    //check if fd is REALLY ready, POLLIN set to 1
    // std::cout << "revents=" << poll_fd[i].revents << " POLLIN=" << POLLIN << std::endl;

    char   msg[1024]; 
    int    read_len  = recv(poll_fd[fd].fd,msg, sizeof(msg)-1,0);
    msg[read_len] = '\0';
    // std::cout << poll_fd[fd].fd << " received: " << msg << std::endl;

    // save message in message_received for that client, mapped via fd
    // poll_client__mapping_via_fd[poll_fd[fd].fd].set_message_received(msg);
    //use class InputBuffer instead of msg received and msg put together
    
    if (read_len > 0)
    {
        msg[read_len] = '\0';
        // std::cout << "current Buffer:     " << poll_clientUser__mapping_via_fd[poll_fd[fd].fd].get_inputBuffer().get_buffer() << std::endl;
        
        //appened to InputBuffer
        // poll_client__mapping_via_fd[poll_fd[fd].fd].get_inputBuffer().append(msg);
          //make clientUser another name for poll_clientUser__mapping_via_fd[poll_fd[fd].fd] and horten this spagethi! We know, I am clever.
        ClientUser &clientUser = poll_clientUser__mapping_via_fd[poll_fd[fd].fd]; 
        clientUser.get_inputBuffer().append(msg);
    
        // std::cout << "Received: " << poll_client__mapping_via_fd[poll_fd[fd].fd].get_message_put_together() << std::endl;
        // std::cout << "recv appended to current Buffer: " << poll_client__mapping_via_fd[poll_fd[fd].fd].get_inputBuffer().get_buffer() << std::endl;
        // std::cout << "recv appended to current Buffer: " << clientUser.get_inputBuffer().get_buffer() << std::endl;
        
    //     handle inputBuffer and execute commands
    //     outside of receive message as the inputBuffer is saved inside each ClientUser    
    //     //parse msg for command
    //     while (clientUser.get_inputBuffer().hasLine())
    //     {
    //         ParsedCommand cmd = Parser::parseLine(clientUser.get_inputBuffer().popLine());
    //         printCommand(cmd);
    //         CommandDispatcher::dispatch(irc_server)
    //     }
    }
    else 
    {   
        if (read_len == 0)
        {
            irc_server.unregisterClientFd(poll_fd[fd].fd);
            std::cout << "Client disconnected fd = " << poll_fd[fd].fd << std::endl;
            poll_clientUser__mapping_via_fd.erase(poll_fd[fd].fd);
            close(poll_fd[fd].fd);
            poll_fd.erase(poll_fd.begin() + fd);
        }
        else // read_len < 0
        {
            if (errno == EAGAIN)
            {
                // no data available right now, keep looping
            }
            else
            {
                // perror("recv");
                // handle error
            }
        }
    }
    return 0;
}

int process_ready_fd(Server &irc_server, std::vector<pollfd> &poll_fd, int fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
{
    // std::cout << "poll fd = " << poll_fd[i].fd<< std::endl;
    if (poll_fd[fd].fd == irc_server.get_server_fd())
    {
        //needs checking return value and error msg on it
        clientUsers_waiting(irc_server, poll_fd, poll_clientUser__mapping_via_fd);
    }
    else
    {
        if (receive_message(irc_server, poll_fd, fd, poll_clientUser__mapping_via_fd) < 0)
        {
            return -1;
        }
        handleClientInput(poll_clientUser__mapping_via_fd[poll_fd[fd].fd], irc_server);
    }
    return 0;
}

static int process_fd_ready_for_sending(Server &irc_server, std::vector<pollfd> &poll_fd, int fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
{
    // std::cout << "poll fd = " << poll_fd[i].fd<< std::endl;
    if (poll_fd[fd].fd != irc_server.get_server_fd())
    {
        //set client to client object that has been mapped to this fd 
        ClientUser &clientUser = poll_clientUser__mapping_via_fd[poll_fd[fd].fd];
        if (sendMsg(clientUser) < 0)
        {
            return -1;
        }
    }
    return 0;
}

int runPoll(Server &irc_server, std::vector<pollfd> &poll_fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
{
        // poll()—Synchronous I/O Multiplexing
        // data() = pointer to first elementS
        int poll__fd_ready__amount = poll(poll_fd.data(), poll_fd.size(), -1);
        //if not fd is ready
        if (poll__fd_ready__amount < 0)
        {
            if (errno == EAGAIN)
            {
                return 0;
            }
            else
            {
                std::cout << "Polling failed." << std::endl;
                return -1;
            }
        }
        // std::cout << "Polling " << poll__fd_ready__amount << std::endl;
        
        for (size_t fd = 0; fd < poll_fd.size(); fd++)
        {
            if (poll_fd[fd].revents & POLLIN)
            {
                if (process_ready_fd(irc_server, poll_fd, fd, poll_clientUser__mapping_via_fd) < 0)
                {
                    return -1;
                }
                poll_fd[fd].revents = 0;
            }

            if (poll_fd[fd].revents & POLLOUT)
            {
                if (process_fd_ready_for_sending(irc_server, poll_fd, fd, poll_clientUser__mapping_via_fd) < 0)
                {
                    return -1;
                }
                poll_fd[fd].revents = 0;
            }
//          POLLIN   → there is data to read
//          POLLOUT  → socket can accept data to send
//          POLLERR  → error
//          POLLHUP  → hangup
        }   
    return 0;
}

void clean_up(std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
{
    //auto &pair figures out the type automatically
    // for (auto &pair : poll_client__mapping_via_fd)
    for (auto &[fd, client] : poll_clientUser__mapping_via_fd)
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
    poll_clientUser__mapping_via_fd.clear(); // triggers destructors
}

int runServer(Server &irc_server)
{
    //create array for fd to poll, to check, if ready with data
    std::vector<pollfd> poll_fd;
    //store server socket fd in poll array
    pollfd   temp;
    temp.fd         = irc_server.get_server_fd();
    // | POLLERR | POLLHUP ...?
    temp.events     = POLLIN | POLLOUT;
    temp.revents    = 0;
    poll_fd.push_back(temp);

    //put into class server
    // //create maping for (unique index) fd to Client object 
    // //unordered map jumps to item by index and is faster than (sorted) mappoll_client__mapping_via_fd;
    // //int is the index which is equal to client_accepted_fd
    // //Client is the type we map to.
    std::unordered_map<int, ClientUser> poll_clientUser__mapping_via_fd;
    
    while (true)
    {
       if (runPoll(irc_server, poll_fd, poll_clientUser__mapping_via_fd) == -1)
    //    if (runPoll(irc_server, poll_fd) == -1)
            // return -1;
            break;
    }
    clean_up(poll_clientUser__mapping_via_fd);
    return 0;
}