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

#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/
#include "inputHandling.hpp"
#include <arpa/inet.h>
#include <csignal>
// #include <cerrno>

extern volatile sig_atomic_t g_shutdown;

// client fd cleanup on discconect/close.
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

// simple first send message
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

// int receive_message(Server &irc_server, int fd, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
int receive_message(Server &irc_server, int fd)
{
    // int polled_fd = poll_fd[fd].fd;

    //check if fd is REALLY ready, POLLIN set to 1
    // std::cout << "revents=" << poll_fd[i].revents << " POLLIN=" << POLLIN << std::endl;

    char   msg[1024]; 
    int    read_len  = recv(irc_server.getPollFD()[fd].fd,msg, sizeof(msg)-1, 0);
    
    if (read_len > 0)
    {
        msg[read_len] = '\0';
        // std::cout << poll_fd[fd].fd << " received: " << msg << std::endl;
        // std::cout << "current Buffer:     " << poll_clientUser__mapping_via_fd[poll_fd[fd].fd].get_inputBuffer().get_buffer() << std::endl;
        
        // appened to InputBuffer
        // poll_client__mapping_via_fd[poll_fd[fd].fd].get_inputBuffer().append(msg);
        // make clientUser another name for poll_clientUser__mapping_via_fd[poll_fd[fd].fd] and horten this spagethi! We know, I am clever.
        ClientUser &clientUser = irc_server.getPoll_clientUser__mapping_via_fd()[irc_server.getPollFD()[fd].fd]; 
        clientUser.get_inputBuffer().append(msg);
    
        // std::cout << "Received: " << poll_client__mapping_via_fd[poll_fd[fd].fd].get_message_put_together() << std::endl;
        // std::cout << "recv appended to current Buffer: " << poll_client__mapping_via_fd[poll_fd[fd].fd].get_inputBuffer().get_buffer() << std::endl;
        // std::cout << "recv appended to current Buffer: " << clientUser.get_inputBuffer().get_buffer() << std::endl;
    }
    else 
    {   
        // 0 means the client closed the connection: irssi does automatically with /quit
        // nc doesn't. you need ctrl+c for that
        // -> need to implement disconnect logic for nc (here?, checking)
        // close only closes after the disconnect from the client, so thats not it for ^
        if (read_len == 0)
        {
            std::cout << "Client disconnected fd = " << irc_server.getPollFD()[fd].fd << std::endl;
            irc_server.unregisterClientFd(irc_server.getPollFD()[fd].fd);
            // irc_server.NicknameUnregister(poll_clientUser__mapping_via_fd[poll_fd[fd].fd].getNickname());
            /* generated:
             * Erasing the ClientUser from the unordered_map invalidates any
             * references or pointers to that element. If code elsewhere stored
             * `&irc_server.getPoll_clientUser__mapping_via_fd()[fd]` then those
             * pointers become dangling after this erase. Ensure that
             * `unregisterClientFd` clears any stored pointers and that no code
             * keeps addresses into the map across inserts that may rehash.
             */
            irc_server.getPoll_clientUser__mapping_via_fd().erase(irc_server.getPollFD()[fd].fd);
            close(irc_server.getPollFD()[fd].fd);
            /* generated:
             * Erasing an element from the `pollfd` vector with `erase(begin()+fd)`
             * shifts later elements left. The outer `for` loop increments its
             * index after this function returns, so the element that moved into
             * the erased slot may be skipped this iteration. Safer options:
             *  - swap the last element into this slot and pop_back (O(1)),
             *    updating an `fd->index` map for lookups; or
             *  - decrement the loop index after erase so the moved element is
             *    processed; or
             *  - break and restart scanning.
             */
            irc_server.getPollFD().erase(irc_server.getPollFD().begin() + fd);
        }
        else // read_len == -1 (< 0)
        {
            // technically if we reach here, recv is -1
            // that means we should probably check properly than this
            if (errno == EAGAIN)
            {
                std::cout << "read_len < 0 error. EAGAIN. Investigative debug print" << std::endl;
                // no data available right now, keep looping
                // return -2 + proper implementation to know what happened?
                // currently: silent continuation
            }
            else
            {
                std::cout << "read_len < 0 error. Unknown. Investigative debug print" << std::endl;
                // perror("recv");
                // handle error
            }
        }
    }
    return 0;
}

int acceptClientUser(Server &irc_server)
{
    //create sockets for our client/user, get fd and some more stuff
    // init server_address as it is a pointer
    // init socklen as it is a pointer here

    socklen_t   client_address_length = sizeof(irc_server.get_server_address());
    sockaddr_in client_address_in = irc_server.get_server_address();

    // the connecting clients assigned fd
    int client_accept_fd = accept(irc_server.get_server_fd(), (struct sockaddr *)&client_address_in, &client_address_length);

    // if the assignment failed
    if (client_accept_fd < 0)
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
    //  melting star prevention, as poll is constantly woken up by check for POLLOUT and thus 100% cpu usage
    // we will enable POLLOUT only when needed by the OutputBuffer
    // temp.events     = POLLIN | POLLOUT;
    temp.events     = POLLIN;
    temp.revents    = 0;
    /* generated:
     * Pushing a new pollfd may reallocate the underlying std::vector buffer.
     * Any pointers or references taken earlier to elements inside
     * `irc_server.getPollFD()` would become dangling after reallocation.
     * Current code uses numeric indexing (`getPollFD()[i]`) which is safe
     * as long as you don't cache `pollfd*` or `pollfd&` across this call.
     * Mitigations: reserve expected capacity at startup or avoid holding
     * long-lived references into the vector. For robust removal consider
     * maintaining an `fd->index` map and using swap-pop erase.
     */
    irc_server.getPollFD().push_back(temp);
    
    // create Client object with its assigned fd
    ClientUser client_created(client_accept_fd);

    // ip is 32bit int, not human readable
    // IPv4 address from network byte order (a binary struct in_addr) to a dotted-decimal string 
    // TODO: return network ip instead of local loopback 
    // (if connecting per localhost or 127.0.0.1, the ip shown to others is localhost/127.0.0.1)
    // it should be the network ip or public ip
    client_created.setIp(inet_ntoa(client_address_in.sin_addr));

    /* generated:
     * Inserting a ClientUser value into `std::unordered_map<int, ClientUser>`
     * stores the object inside the map's buckets. Taking the address of the
     * stored element (as below) and storing it elsewhere is dangerous because
     * `unordered_map` will rehash and relocate elements when it grows,
     * invalidating references/pointers to those elements.
     * Current behaviour copies the client into the map and then passes
     * `&map[fd]` into `registerClientFd`. This is safe only until the map
     * rehashes. Possible mitigations: reserve enough buckets at startup,
     * store `unique_ptr<ClientUser>` in the map and keep raw pointers to
     * `unique_ptr::get()`, or avoid storing pointers into the map entirely.
     */
    // poll_clientUser__mapping_via_fd[client_accept_fd] = client_created;
    // so we assign a copy of the created client in the irc_server object for our polling logic?
    irc_server.getPoll_clientUser__mapping_via_fd()[client_accept_fd] = client_created;

    // and here we registered it to _clients[n], which is indexing client by fd
    // _client[fd] = client;
    irc_server.registerClientFd(client_accept_fd, &irc_server.getPoll_clientUser__mapping_via_fd()[client_accept_fd]);

    return 0;
}

// accept all clients that are to be ready as per poll(fd) desicion
int clientUsers_waiting(Server &irc_server)
{
    bool clientUsers_are_waiting = true;

    while (clientUsers_are_waiting)
    {       
        // create sockets for our client/user, create Client object and create their fd.
        // accept_client return 0 -> accepted successfully
        switch (acceptClientUser(irc_server))
        {
        case -1:
            return -1; // failed to accept client
        case -2:
            // no more clients are waiting right now, so we leave the unlimited loop to not block
            // returns 0
            clientUsers_are_waiting = false; 
        }
    }
    
    // why are we returning at all? we don't use the return value for anything
    return 0;
}

int process_ready_fd(Server &irc_server, int fd)
{
    // std::cout << "poll fd = " << poll_fd[i].fd<< std::endl;
    if (irc_server.getPollFD()[fd].fd == irc_server.get_server_fd())
    {
        // checks if any connection is waiting
        clientUsers_waiting(irc_server);
    }
    else
    {
        // TODO: receive_message never returns -1 at this moment
        if (receive_message(irc_server, fd) < 0)
        {
            // read_len < 0 check basically 
            // returning -1 would break the loop
            // additional check for -2 (EAGAIN)?
            return -1;
        }
        // mislabeling:
        // it parses the input and then dispatches the input
        handleClientInput(irc_server.getPoll_clientUser__mapping_via_fd()[irc_server.getPollFD()[fd].fd], irc_server);
    }
    return 0;
}

// int runPoll(Server &irc_server, std::unordered_map<int, ClientUser> &poll_clientUser__mapping_via_fd)
int runPoll(Server &irc_server)
{
        // poll()—Synchronous I/O Multiplexing
        // data() = pointer to first elementS

        // poll returns a pointer to the first fd element
        // nfds (pollfd.size) is the count of entries
        // -1 is the timeout -> block until events occur.
        int poll__fd_ready__amount = poll(irc_server.getPollFD().data(), irc_server.getPollFD().size(), -1);
        // error handling
        if (poll__fd_ready__amount < 0)
        {
            // we return to the start of the loop and poll again
            if (errno == EAGAIN || errno == EINTR)
            {
                return 0;
            }
            // else if polling failed we break and exit the program
            else
            {
                std::cout << "Polling failed." << std::endl;
                return -1;
            }
        }
        // std::cout << "Polling " << poll__fd_ready__amount << std::endl; // debug comment
        
        // iterating all fd's as long fd is within size()
        for (size_t fd = 0; fd < irc_server.getPollFD().size(); fd++)
        {
            // TODO POLLERR, POLLNVAL, PULLHUP (at this position)
            // error handling first is safer
            /* generated:
             * The kernel may set `POLLERR`, `POLLHUP` or `POLLNVAL` in
             * `revents` to indicate an error, a hangup, or an invalid fd.
             * These conditions should be checked and handled before
             * attempting `POLLIN`/`POLLOUT` I/O. Failing to do so can lead
             * to attempted reads/writes on sockets that are closed or in an
             * error state. Typical handling pattern:
             *
             *   if (p.revents & (POLLERR | POLLHUP | POLLNVAL)) {
             *       // cleanup: unregister, close, erase pollfd entry
             *       continue; // skip further processing for this fd
             *   }
             *
             * Keep the cleanup deterministic: remove pollfd via swap-pop or
             * otherwise ensure the outer loop handles shifted indices safely.
             */

            // bitwise AND check if POLLIN bit is inside .revents
            // if it is -> true
            // true means -> socket is ready for reading.
            // POLLIN -> socket read check
            if (irc_server.getPollFD()[fd].revents & POLLIN)
            {
                // reading data from client
                // currently never < 0
                if (process_ready_fd(irc_server, fd) < 0)
                {
                    // break the loop
                    return -1;
                }
                irc_server.getPollFD()[fd].revents = 0;
            }

            // POLLOUT -> socket write check
            // start nsloniow 2603171812
            // set POLLOUT on outputbuffer not empty so to tell kernel to try to send from that fd
            // if empty no need to use cpu to tell kernel that something for sending is waiting on that fd
            if (irc_server.getPollFD()[fd].fd != irc_server.get_server_fd())
            {
                ClientUser &client_for_current_fd = irc_server.getPoll_clientUser__mapping_via_fd()[irc_server.getPollFD()[fd].fd];
                if (!client_for_current_fd.get_outputBuffer().get_buffer().empty())
                    irc_server.getPollFD()[fd].events |= POLLOUT;
            // end nsloniow 2603171812
                if (irc_server.getPollFD()[fd].revents & POLLOUT)
                {
                    // if (process_fd_ready_for_sending(irc_server, fd, irc_server.getPoll_clientUser__mapping_via_fd()) < 0)
                    if (process_fd_ready_for_sending(irc_server, fd) < 0)
                    {
                        return -1;
                    }
                    irc_server.getPollFD()[fd].revents = 0;
                }
            // start nsloniow 2603171812
            // set POLLOUT on outputbuffer not empty so to tell kernel to try to send from that fd
            // if empty no need to use cpu to tell kernel that something for sending is waiting on that fd
                if (client_for_current_fd.get_outputBuffer().get_buffer().empty())
                    irc_server.getPollFD()[fd].events &= ~POLLOUT;
            }
            // end nsloniow 2603171812
        //          POLLIN   → there is data to read
        //          POLLOUT  → socket can accept data to send
        //          POLLERR  → error
        //          POLLHUP  → hangup
        }   
    return 0;
}

void runServer(Server &irc_server)
{
    pollfd          temp;
    temp.fd         = irc_server.get_server_fd();
    // | POLLERR | POLLHUP ...?
    temp.events     = POLLIN | POLLOUT;
    temp.revents    = 0;
    irc_server.getPollFD().push_back(temp);

    while (!g_shutdown)
    {
        if (runPoll(irc_server) == -1)
            break;
    }
    std::cout << "\n" << "Graceful shutdown" << std::endl;
    clean_up(irc_server.getPoll_clientUser__mapping_via_fd());
}

