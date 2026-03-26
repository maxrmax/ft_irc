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
#include <sys/resource.h>
#include <arpa/inet.h>
#include <algorithm>
#include <csignal>
// #include <cerrno>

extern volatile sig_atomic_t g_shutdown;

// client fd cleanup on discconect/close.
void clean_up(std::unordered_map<int, ClientUser> &clients)
{
    //auto &pair figures out the type automatically
    // for (auto &pair : poll_client__mapping_via_fd)
    for (auto &[fd, client] : clients)
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
    clients.clear(); // triggers destructors
}

// simple first send message
static int sendMsg(ClientUser *clientUser)
{    
    if (clientUser->get_outputBuffer().get_buffer().length() > 0)
    {
        // ssize_t send(int __fd, const void *__buf, size_t __n, int __flags)
        // Send N bytes of BUF to socket FD. Returns the number sent or -1.
        // This function is a cancellation point and therefore not marked with __THROW.
        ssize_t send_len = send(clientUser->get_ClientUser_fd(), clientUser->get_outputBuffer().get_buffer().c_str(), clientUser->get_outputBuffer().get_buffer().size(), 0);
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
            clientUser->get_outputBuffer().get_buffer().erase(0, send_len);
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
        ClientUser *clientUser = irc_server.getClientByFd(fd);
        if (sendMsg(clientUser) < 0)
        {
            return -1;
        }
    }
    return 0;
}

int receive_message(Server &irc_server, int fd)
{
    pollfd polled_fd = irc_server.getPollFD()[fd];

    //check if fd is REALLY ready, POLLIN set to 1
    std::cout << "revents=" << polled_fd.revents << " POLLIN=" << POLLIN << std::endl;

    char   msg[1024]; 
    int    read_len  = recv(polled_fd.fd, msg, sizeof(msg)-1, 0);
    
    if (read_len > 0)
    {
        msg[read_len] = '\0';
        
        // appened to InputBuffer
        ClientUser *clientUser = irc_server.getClientByFd(fd);
        clientUser->get_inputBuffer().append(msg);
    
        std::cout << "Received: " << msg << std::endl;
        std::cout << "Current Buffer: " << clientUser->get_inputBuffer().get_buffer() << std::endl;
        std::cout << "Appended Current Buffer: " << clientUser->get_inputBuffer().get_buffer() << std::endl;
    }
    else 
    {   
        // 0 means the client closed the connection: irssi does automatically with /quit
        // nc doesn't. you need ctrl+c for that
        // -> need to implement disconnect logic for nc (here?, checking)
        // close only closes after the disconnect from the client, so thats not it for ^
        if (read_len == 0)
        {
            std::cout << "Client disconnected fd = " << polled_fd.fd << std::endl;
            // finds current fd in _clients
            irc_server.unregisterClientFd(polled_fd.fd);
            /* generated:
             * Erasing the ClientUser from the unordered_map invalidates any
             * references or pointers to that element.Ensure that
             * `unregisterClientFd` clears any stored pointers and that no code
             * keeps addresses into the map across inserts that may rehash.
             */
            // TODO
            // check where i have to remove fd from after unregister
            // unregister -> remove from _clients;
            // poll_fd?
            // nickname_history?
            // nick_clientUser?
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

// Core logic around fd assignment and creation of client object in irc_server (_clients)
int acceptClientUser(Server &irc_server)
{
    // get the incoming ip
    // ??? why do we take the server ip, where is the client ip?
    sockaddr_in client_address_in = irc_server.get_server_address();
    // get address length
    socklen_t   client_address_length = sizeof(client_address_in);

    // the connecting clients assigned fd
    int client_accept_fd = accept(irc_server.get_server_fd(), (struct sockaddr *)&client_address_in, &client_address_length);

    // if the assignment failed
    if (client_accept_fd < 0)
    {
        // non-blocking needs check on errno so it keeps looping until we finish our loop
        if (errno == EAGAIN)
        {
            return -2; // sets client_waiting = false
        }
        else
        {
            std::cout << "Client acception failed." << std::endl;
            return -1;
        }
    }
    std::cout << "runServer: Client accepted fd = " << client_accept_fd << std::endl;
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
    // gets destroyed on return, is only alive inside this function
    // ClientUser client_created(client_accept_fd); // 

    // new user register instead of stack creation
    irc_server.registerClientFd(client_accept_fd);
    // ClientUser *client_ptr = irc_server.getClientByFd(client_accept_fd);
    // if (client_ptr) client_ptr->setIp(inet_ntoa(client_address_in.sin_addr));
    // std::cout << "runServer: Client " << client_accept_fd << " ip: " << (client_ptr ? client_ptr->getIp() : std::string()) << std::endl;



    // and here we registered it to _clients[n], which is indexing client by fd
    // _client[fd] = client; irc_server.registerClientFd(client_accept_fd, std::move(client_created));
    
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
    // irc_server.registerClientFd(client_accept_fd, std::move(client_created)); // old
    std::cout << "runServer: Client constructed: " << client_accept_fd << std::endl;


    // ip is 32bit int, not human readable
    // IPv4 address from network byte order (a binary struct in_addr) to a dotted-decimal string 
    // TODO: return network ip instead of local loopback 
    // (if connecting per localhost or 127.0.0.1, the ip shown to others is localhost/127.0.0.1)
    // it should be the network ip or public ip
    irc_server.setClientIp(client_accept_fd, inet_ntoa(client_address_in.sin_addr));
/*
    // If client connected via localhost, advertise the server's outbound
    // IP (the interface used to reach the internet) so remote peers can
    // reach DCC/file transfers. Use a non-sending UDP trick to discover
    // the local outbound address. Keep minimal: compute once per process.
    static const std::string server_public = []() -> std::string
    {
        int s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0) return std::string();
        sockaddr_in remote{};
        remote.sin_family = AF_INET;
        remote.sin_port = htons(53);
        inet_pton(AF_INET, "8.8.8.8", &remote.sin_addr);
        connect(s, (sockaddr*)&remote, sizeof(remote));
        sockaddr_in local{};
        socklen_t len = sizeof(local);
        if (getsockname(s, (sockaddr*)&local, &len) < 0) { close(s); return std::string(); }
        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &local.sin_addr, buf, sizeof(buf));
        close(s);
        return std::string(buf);
    }();

    char peerbuf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_address_in.sin_addr, peerbuf, sizeof(peerbuf));
    if (strncmp(peerbuf, "127.", 4) == 0 || client_address_in.sin_addr.s_addr == htonl(INADDR_LOOPBACK))
        client_created.setIp(server_public.empty() ? std::string(peerbuf) : server_public);
    else
        client_created.setIp(std::string(peerbuf));
    */

    std::cout << "runServer: Client " << client_accept_fd << " ip: " << irc_server.getClientIp(client_accept_fd) << std::endl;
    return 0;
}

// accept all clients that are to be ready as per poll(fd) desicion
int clientUsers_waiting(Server &irc_server)
{
    // just set to true, why not.
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

// process the poll vector poll_fd
int process_ready_fd(Server &irc_server, int fd)
{
    // std::cout << "poll fd = " << poll_fd[i].fd<< std::endl;
    // if current polled_fd is the servers fd, check if any connection is waiting.
    if (fd == irc_server.get_server_fd())
    {
        // checks if any connection is waiting
        clientUsers_waiting(irc_server);
    }
    else // else handle clients polled_fd and their input
    {
        // TODO: receive_message never returns -1 at this moment
        receive_message(irc_server, fd);
        // mislabeling:
        // it parses the input and then dispatches the input
        // command-dispatcher "handleClientCommands"
        handleClientInput(irc_server.getClientByFd(fd), irc_server);
    }
    return 0;
}

int runPoll(Server &irc_server)
{
        // poll()—Synchronous I/O Multiplexing
        // data() = pointer to first element

        // nfds (pollfd.size) is the count of entries
        // -1 is the timeout -> block until events occur

        // poll returns number based on:
        // 1+ (amount of fds with events)
        // 0 for timeout
        // -1 for error (example: nothing to poll -> EAGAIN)
        int poll_amount = poll(irc_server.getPollFD().data(), irc_server.getPollFD().size(), -1);
        if (poll_amount < 0)
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
        std::cout << "Polling " << poll_amount << std::endl; // debug comment
        
        // iterating all found fd's as long fd is within poll list
        // from here on we handle a single fd/client per iteration.
        // and where do i push fd's into the vector list?
        // -> process_ready_fd

        // double check fd (iterator) with every client_poll.fd
        for (size_t fd = 0; fd < irc_server.getPollFD().size(); fd++)
        {
            std::cout << "for fd: " << fd << " iterator" << std::endl;
            pollfd client_poll = irc_server.getPollFD()[fd];

            if (client_poll.revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                // TODO: cleanup: unregister, close, erase pollfd entry
                // continue; // skip further processing for this fd
            }
        
            //  * Keep the cleanup deterministic: remove pollfd via swap-pop or
            //  * otherwise ensure the outer loop handles shifted indices safely.

            // bitwise AND check if POLLIN bit is inside .revents
            // if it is -> true
            // true means -> socket is ready for reading.
            // POLLIN -> socket read check
            if (client_poll.revents & POLLIN)
            {
                // reading data from client
                // alwyas returns 0 currently

                // waiting clients
                // receive_message()
                // handleClientInput -> dispatch -> commands
                process_ready_fd(irc_server, client_poll.fd);
                client_poll.revents = 0;
            }

            // POLLOUT -> socket write check
            // set POLLOUT on outputbuffer to non empty, to tell the kernel to try to send from that fd
            if (client_poll.fd != irc_server.get_server_fd())
            {
                // client pointer
                ClientUser *client_for_current_fd = irc_server.getClientByFd(client_poll.fd);

                // if its not empty set pollout in .events.
                if (!client_for_current_fd->get_outputBuffer().get_buffer().empty())
                    client_poll.events |= POLLOUT;

                // if pollout is set in .revents
                if (client_poll.revents & POLLOUT)
                {
                    // process the msg to send.
                    if (process_fd_ready_for_sending(irc_server, client_poll.fd) < 0)
                    {
                        return -1;
                    }
                    client_poll.revents = 0;
                }

                // set POLLOUT on outputbuffer not empty so to tell kernel to try to send from that fd
                // if empty no need to use cpu to tell kernel that something for sending is waiting on that fd
                // if outputBuffer is empty, &= ~POLLOUT
                if (client_for_current_fd->get_outputBuffer().get_buffer().empty())
                    client_poll.events &= ~POLLOUT;
            }
        }   
    return 0;
}

void init_server_limits(Server &irc_server)
{
    // reserver fd's for other programs
    const int reserved_fds = 10;
    // set a real limit
    struct rlimit rl;
    // soft limit
    size_t soft_limit = 0;
    // max sockets we can open by the system
    long sc = sysconf(_SC_OPEN_MAX);
    size_t max_clients = 0;

    // we get the soft limit
    if (getrlimit(RLIMIT_NOFILE, &rl) != 0 || rl.rlim_cur == RLIM_INFINITY)
        soft_limit = (sc > 0) ? (size_t)sc : 1024;
    else
        soft_limit = (size_t)rl.rlim_cur;

    // we define the max_clients to allow to connect
    if (soft_limit > (size_t)reserved_fds)
        max_clients = soft_limit - (size_t)reserved_fds;
    if (max_clients < 16)
        max_clients = 16;

    std::cout << "max_clients: " << max_clients << std::endl; // debug print
    // Reserve space to avoid rehash/realloc during runtime.
    irc_server.getPollFD().reserve(max_clients + 1); // +1 for server/listener fd
    // also reserve the space on _clients map
    irc_server.get_clients_map().reserve(max_clients);
}

void runServer(Server &irc_server)
{
    // datastructure of a single fd with events (the polling fd)
    pollfd                  polling_server_fd;
    // first set server fd to poll list for server events
    polling_server_fd.fd         = irc_server.get_server_fd();
    // | POLLERR | POLLHUP ...?
    // define the events we want the server to react to
    polling_server_fd.events     = POLLIN | POLLOUT;
    // set to "no events have happened"
    polling_server_fd.revents    = 0;
    // push the server fd on the stack
    irc_server.getPollFD().push_back(polling_server_fd);

    // init the server
    init_server_limits(irc_server);

    while (!g_shutdown)
    {
        if (runPoll(irc_server) == -1)
            break;
    }
    std::cout << "\n" << "Graceful shutdown" << std::endl;
    clean_up(irc_server.get_clients_map());
}

