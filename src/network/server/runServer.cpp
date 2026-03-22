/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:47:16 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/21 12:26:22 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// runServer.cpp

#include "../../../includes/ircserv.hpp"
#include <arpa/inet.h> 

extern volatile sig_atomic_t g_shutdown;


//simple first send message
static int sendMsg(ClientUser &clientUser)
{    
    if (clientUser.get_outputBuffer().get_buffer().length() > 0)
    {
        ssize_t send_len = send(clientUser.get_ClientUser_fd(), clientUser.get_outputBuffer().get_buffer().c_str(), clientUser.get_outputBuffer().get_buffer().size(), 0);
        // printf("%s %d send_len %zd\n", __FILE__, __LINE__, send_len);
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
            // printf("%s %d outBuff BEFORE pop %s\n", __FILE__, __LINE__, clientUser.get_outputBuffer().get_buffer().c_str());
            // clientUser.get_outputBuffer().popLine();
            clientUser.get_outputBuffer().get_buffer().erase(0, send_len);
            // printf("%s %d outBuff AFTER  pop %s\n", __FILE__, __LINE__, clientUser.get_outputBuffer().get_buffer().c_str());
        }
        // std::cout << "out after send: " << clientUser.get_outputBuffer().get_buffer() << std::endl;
    }
    return 0;
}

// int acceptClientUser(Server &irc_server, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
int acceptClientUser(Server &irc_server)
{
    //create sockets for our client/user, get fd and some more stuff
    // init server_address as it is a pointer
    // init socklen as it is a pointer here
    // socklen_t server_address_length = sizeof(server_address);
    // socklen_t server_address_length = sizeof(irc_server.get_server_address());
    socklen_t   client_address_length = sizeof(irc_server.get_server_address());
    sockaddr_in client_address_in = irc_server.get_server_address();
    int client_accept_fd = accept(irc_server.get_server_fd(), (struct sockaddr *)&client_address_in, &client_address_length);

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
    //melting star prevention, as poll is constantly woken up by check for POLLOUT
    //and thus 100% cpu usage
    //we will enable POLLOUT when we put something in the OutputBuffer
    // temp.events     = POLLIN | POLLOUT;
    temp.events     = POLLIN;
    temp.revents    = 0;
    std::cout << __FILE__ << __LINE__ << std::endl;
    irc_server.getPollFD().push_back(temp);
    
    //create Client object
    ClientUser client_created(client_accept_fd);
    //ip is 32bit int, not human readable
    //IPv4 address from network byte order (a binary struct in_addr) to a dotted-decimal string 
    // TODO: return network ip instead of local loopback 
    // (if connecting per localhost or 127.0.0.1, the ip shown to others is localhost/127.0.0.1)
    // it should be the network ip or public ip
    client_created.setIp(inet_ntoa(client_address_in.sin_addr));

    // poll_clientUser__mapping_via_fd[client_accept_fd] = client_created;
    irc_server.getPoll_clientUser__mapping_via_fd()[client_accept_fd] = client_created;
    // irc_server.registerClientFd(client_accept_fd, &poll_clientUser__mapping_via_fd[client_accept_fd]);
    irc_server.registerClientFd(client_accept_fd, &irc_server.getPoll_clientUser__mapping_via_fd()[client_accept_fd]);
    //we do not create client here

    return 0;
}

// int clientUsers_waiting(Server &irc_server, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
int clientUsers_waiting(Server &irc_server)
{
    // accept all clients that are to be ready as per poll(fd) desicion
    bool clientUsers_are_waiting = true;
    while (clientUsers_are_waiting)
    {       
        //create sockets for our client/user, get fd and some more stuff
        //accept_client return 0 -> accepted successfully
        // switch (acceptClientUser(irc_server, irc_server.getPoll_clientUser__mapping_via_fd()))
        switch (acceptClientUser(irc_server))
        {
        case -1:
            return -1; // acception faild
        case -2:
            clientUsers_are_waiting = false; //no more clients are waiting right now, so we leave the unlimited loop to not block
        }
    }
    
    return 0;
}


// int receive_message(Server &irc_server, int fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
int receive_message(Server &irc_server, int fd)
{
    // int polled_fd = poll_fd[fd].fd;

    //check if fd is REALLY ready, POLLIN set to 1
    // std::cout << "revents=" << poll_fd[i].revents << " POLLIN=" << POLLIN << std::endl;

    char   msg[1024]; 
    int    read_len  = recv(irc_server.getPollFD()[fd].fd,msg, sizeof(msg)-1,0);
    
    if (read_len > 0)
    {
        msg[read_len] = '\0';
        // std::cout << poll_fd[fd].fd << " received: " << msg << std::endl;
        // std::cout << "current Buffer:     " << poll_clientUser__mapping_via_fd[poll_fd[fd].fd].get_inputBuffer().get_buffer() << std::endl;
        
        //appened to InputBuffer
        // poll_client__mapping_via_fd[poll_fd[fd].fd].get_inputBuffer().append(msg);
        //make clientUser another name for poll_clientUser__mapping_via_fd[poll_fd[fd].fd] and horten this spagethi! We know, I am clever.
        // ClientUser &clientUser = poll_clientUser__mapping_via_fd[irc_server.getPollFD()[fd].fd]; 
        ClientUser &clientUser = irc_server.getPoll_clientUser__mapping_via_fd()[irc_server.getPollFD()[fd].fd]; 
        clientUser.get_inputBuffer().append(msg);
    
        // std::cout << "Received: " << poll_client__mapping_via_fd[poll_fd[fd].fd].get_message_put_together() << std::endl;
        // std::cout << "recv appended to current Buffer: " << poll_client__mapping_via_fd[poll_fd[fd].fd].get_inputBuffer().get_buffer() << std::endl;
        // std::cout << "recv appended to current Buffer: " << clientUser.get_inputBuffer().get_buffer() << std::endl;

        return 1;
    }
    else 
    {   
        if (read_len == 0)
        {
            std::cout << "Client disconnected fd = " << irc_server.getPollFD()[fd].fd << std::endl;
            irc_server.unregisterClientFd(irc_server.getPollFD()[fd].fd);
            std::cout << __FILE__ << __LINE__ << std::endl;
            // irc_server.NicknameUnregister(poll_clientUser__mapping_via_fd[poll_fd[fd].fd].getNickname());
            // poll_clientUser__mapping_via_fd.erase(irc_server.getPollFD()[fd].fd);
            irc_server.getPoll_clientUser__mapping_via_fd().erase(irc_server.getPollFD()[fd].fd);
            std::cout << __FILE__ << __LINE__ << std::endl;
            close(irc_server.getPollFD()[fd].fd);
            std::cout << __FILE__ << __LINE__ << std::endl;
            irc_server.getPollFD().erase(irc_server.getPollFD().begin() + fd);
            std::cout << __FILE__ << __LINE__ << std::endl;
            return -1;
        }
        else // read_len < 0
        {
            if (errno == EAGAIN)
            {
                // no data available right now, keep looping
                return 0;
            }
            else
            {
                // perror("recv");
                // handle error
                return -1;
            }
        }
    }
    return 0;
}

// int process_ready_fd(Server &irc_server, int fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
int process_ready_fd(Server &irc_server, int fd)
{
    // std::cout << "poll fd = " << poll_fd[i].fd<< std::endl;
    if (irc_server.getPollFD()[fd].fd == irc_server.get_server_fd())
    {
        //needs checking return value and error msg on it
        // clientUsers_waiting(irc_server, irc_server.getPoll_clientUser__mapping_via_fd());
        clientUsers_waiting(irc_server);
    }
    else
    {
        // if (receive_message(irc_server, fd, irc_server.getPoll_clientUser__mapping_via_fd()) < 0)
        if (receive_message(irc_server, fd) < 0)
        {
            std::cout << __FILE__ << __LINE__ << std::endl;
            return -1;
        }
        std::cout << __FILE__ << __LINE__ << std::endl;
        handleClientInput(irc_server.getPoll_clientUser__mapping_via_fd()[irc_server.getPollFD()[fd].fd], irc_server);
        std::cout << __FILE__ << __LINE__ << std::endl;
    }
    return 0;
}

// static int process_fd_ready_for_sending(Server &irc_server, int fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
static int process_fd_ready_for_sending(Server &irc_server, int fd)
{
    // std::cout << "poll fd = " << poll_fd[i].fd<< std::endl;
    if (irc_server.getPollFD()[fd].fd != irc_server.get_server_fd())
    {
        //set client to client object that has been mapped to this fd 
        ClientUser &clientUser = irc_server.getPoll_clientUser__mapping_via_fd()[irc_server.getPollFD()[fd].fd];
        if (sendMsg(clientUser) < 0)
        {
            return -1;
        }
    }
    return 0;
}

/*
TODO: runServer.cpp disconnect/poll refactor checklist

1) Centralize disconnect flow
   - Create one helper for: unregisterClientFd(fd), erase client map entry, close(fd), erase pollfd index.
   - Reuse this helper for both QUIT-triggered disconnect and recv()==0 (EOF).

2) Return status when current poll index is erased
   - receive_message/process_ready_fd should return:
     - <0 on error
     - >0 when current fd was removed from poll list
     - 0 otherwise
   - Prevent continuing to use poll_fd[fd] after erase.

3) Avoid invalid access after erase
   - Current flow may call handleClientInput(...) after receive_message removed poll_fd[fd].
   - Guard against stale index/out-of-bounds and stale map lookups.

4) Handle QUIT transport teardown in network layer
   - CmdQuit may unregister server state, but socket still needs close + poll erase here.
   - After handleClientInput(...), detect "client no longer registered" and disconnect current fd.

5) Fix runPoll iteration when erasing elements
   - Do not use unconditional ++fd after erase of current index.
   - Use manual increment strategy with an "erasedCurrent" flag.

6) Avoid accidental map insertion via operator[]
   - Replace poll_clientUser__mapping_via_fd[fd] reads with find()/at() where possible.
   - Prevent recreating client entries after disconnect paths.

7) Keep EOF and QUIT behavior identical
   - Same teardown path for recv()==0 and QUIT.
   - Ensures consistent cleanup of channels, nick maps, socket, and poll vector.

8) Optional safety checks
   - Handle POLLERR/POLLHUP/POLLNVAL with same disconnect helper.
   - Ensure close(fd) is not called twice in shutdown/cleanup paths.
*/

// int runPoll(Server &irc_server, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
int runPoll(Server &irc_server)
{
        // poll()—Synchronous I/O Multiplexing
        // data() = pointer to first elementS

        int poll__fd_ready__amount = poll(irc_server.getPollFD().data(), irc_server.getPollFD().size(), -1);
        //if not fd is ready
        if (poll__fd_ready__amount < 0)
        {
            if (errno == EAGAIN || errno == EINTR)
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
        
        for (size_t fd = 0; fd < irc_server.getPollFD().size(); fd++)
        {
            if (irc_server.getPollFD()[fd].revents & POLLIN)
            {
                // reading data from client(s)
                // if (process_ready_fd(irc_server, fd, poll_clientUser__mapping_via_fd) < 0)
                if (process_ready_fd(irc_server, fd) < 0)
                {
                    return -1;
                }
                irc_server.getPollFD()[fd].revents = 0;
            }

            //start nsloniow 2603171812
            //set POLLOUT on outputbuffer not empty so to tell kernel to try to send from that fd
            //if empty no need to use cpu to tell kernel that something for sending is waiting on that fd
            if (irc_server.getPollFD()[fd].fd != irc_server.get_server_fd())
            {
                ClientUser &client_for_current_fd = irc_server.getPoll_clientUser__mapping_via_fd()[irc_server.getPollFD()[fd].fd];
                if (!client_for_current_fd.get_outputBuffer().get_buffer().empty())
                    irc_server.getPollFD()[fd].events |= POLLOUT;
            //end nsloniow 2603171812
                if (irc_server.getPollFD()[fd].revents & POLLOUT)
                {
                    // if (process_fd_ready_for_sending(irc_server, fd, irc_server.getPoll_clientUser__mapping_via_fd()) < 0)
                    if (process_fd_ready_for_sending(irc_server, fd) < 0)
                    {
                        return -1;
                    }
                    irc_server.getPollFD()[fd].revents = 0;
                }
            //start nsloniow 2603171812
            //set POLLOUT on outputbuffer not empty so to tell kernel to try to send from that fd
            //if empty no need to use cpu to tell kernel that something for sending is waiting on that fd
                if (client_for_current_fd.get_outputBuffer().get_buffer().empty())
                    irc_server.getPollFD()[fd].events &= ~POLLOUT;
            }
            //end nsloniow 2603171812
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
                // std::cout << "Client filedescriptor closed for a good nights sleep." << std::endl;
                std::cout << "Client filedescriptor closed" << std::endl;
            }
            else 
            {
                std::cout << "Client filedescriptor could not be closed. Operating System will do this." << std::endl;
            }
        }       
    }
    poll_clientUser__mapping_via_fd.clear(); // triggers destructors
}

void runServer(Server &irc_server)
{
    //move into class Server
    //create array for fd to poll, to check, if ready with data
    // std::vector<pollfd> poll_fd;
    //store server socket fd in poll array
    pollfd   temp;
    temp.fd         = irc_server.get_server_fd();
    // | POLLERR | POLLHUP ...?
    temp.events     = POLLIN | POLLOUT;
    temp.revents    = 0;
    // poll_fd.push_back(temp);
    irc_server.getPollFD().push_back(temp);

    //move into class server
    //create maping for (unique index) fd to Client object 
    //unordered map jumps to item by index and is faster than (sorted) mappoll_client__mapping_via_fd;
    //int is the index which is equal to client_accepted_fd
    //Client is the type we map to.
    // std::unordered_map<int, ClientUser> poll_clientUser__mapping_via_fd;
    
    while (!g_shutdown)
    {
        //EINVAL    -> invalid arguments
        //EBADF     -> broken fd    
        // if (runPoll(irc_server, poll_fd, poll_clientUser__mapping_via_fd) == -1)
        // if (runPoll(irc_server, poll_clientUser__mapping_via_fd) == -1)
        if (runPoll(irc_server) == -1)
            break;
    }
    std::cout << "\n" << "Graceful shutdown" << std::endl;
    clean_up(irc_server.getPoll_clientUser__mapping_via_fd());
    // return 0;
}

