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
    for (auto &[fd, client] : clients)
    {
        // Client destructor will close fd
        // or explicitly close here if you remove it from destructor
        if (fd != -1)
        {
            if (close(fd) == 0)
                std::cout << "Client filedescriptor closed" << std::endl;
            else 
                std::cout << "Client filedescriptor could not be closed. Operating System will do this." << std::endl;
        }       
    }
    clients.clear();
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
static int process_fd_ready_for_sending(Server &irc_server, int poll_index)
{
    // std::cout << "poll fd = " << poll_fd[i].fd<< std::endl;
    int client_fd = irc_server.getPollFD()[poll_index].fd;
    if (client_fd != irc_server.get_server_fd())
    {
        //set client to client object that has been mapped to this fd 
        ClientUser *clientUser = irc_server.getClientByFd(client_fd);
        if (!clientUser)
            return -1;
        if (sendMsg(clientUser) < 0)
        {
            return -1;
        }
    }
    return 0;
}

int receive_message(Server &irc_server, int poll_index)
{
    int client_fd = irc_server.getPollFD()[poll_index].fd;
    ClientUser *clientUser = irc_server.getClientByFd(client_fd);
    if (!clientUser)
        return 0;

    char   msg[1024]; 
    int    read_len  = recv(client_fd, msg, sizeof(msg)-1, 0);
    
    if (read_len > 0)
    {
        msg[read_len] = '\0';
        
        // appened to InputBuffer
        clientUser->get_inputBuffer().append(msg);
    
        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "Received: " << msg << std::endl;
            std::cout << "Current Buffer: " << clientUser->get_inputBuffer().get_buffer() << std::endl;
            std::cout << "Appended Current Buffer: " << clientUser->get_inputBuffer().get_buffer() << std::endl;
        #endif
    }
    else // read_len <= 0
    {   
        // 0 means the client closed the connection: irssi does automatically with /quit
        // nc doesn't. you need ctrl+c for that
        // -> need to implement disconnect logic for nc (here?, checking)
        // close only closes after the disconnect from the client, so thats not it for ^
        if (read_len == 0)
        {
            std::cout << "Client " << client_fd << " disconnected." << std::endl;
            // mark client for disconnect; runPoll will perform cleanup
            clientUser->setToDisconnect(true);
            return -1;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) // read_len < 0
        {
            // no data available right now, keep looping
            return 0;
        }

        // fatal socket error
        std::cerr << "recv error on fd " << client_fd << ": " << strerror(errno) << std::endl;
            // mark client for disconnect; runPoll will perform cleanup
        clientUser->setToDisconnect(true);
        return -1;
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

    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "runServer: Client accepted fd = " << client_accept_fd << std::endl;
    #endif

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

    irc_server.getPollFD().push_back(temp);

    // new user register instead of stack creation
    irc_server.registerClientFd(client_accept_fd);

    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "runServer: Client constructed: " << client_accept_fd << " with ip: " << irc_server.getClientIp(client_accept_fd) << std::endl;
    #endif


    // ip is 32bit int, not human readable
    // IPv4 address from network byte order (a binary struct in_addr) to a dotted-decimal string 
    // TODO: return network ip instead of local loopback 
    // (if connecting per localhost or 127.0.0.1, the ip shown to others is localhost/127.0.0.1)
    // it should be the network ip or public ip
    /*
        use server outbound ip for localhost clients.
        if peerbuf is loopback setClientip to server_public ip.
    */
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
   
    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "runServer: Client " << client_accept_fd << " ip: " << irc_server.getClientIp(client_accept_fd) << std::endl;
    #endif
    
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
int process_ready_fd(Server &irc_server, int poll_index)
{
    int client_fd = irc_server.getPollFD()[poll_index].fd;
    // std::cout << "poll fd = " << poll_fd[i].fd<< std::endl;
    // if current polled_fd is the servers fd, check if any connection is waiting.
    if (client_fd == irc_server.get_server_fd())
    {
        // checks if any connection is waiting
        clientUsers_waiting(irc_server);
    }
    else // else handle clients polled_fd and their input
    {
        // TODO: receive_message never returns -1 at this moment
        if (receive_message(irc_server, poll_index) == -1)
            return -1;
        ClientUser *cu = irc_server.getClientByFd(client_fd);
        if (!cu)
            return 0;

        // it parses the input and then dispatches the input
        // command-dispatcher "handleClientCommands"
        handleClientInput(cu, irc_server);
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
        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "Polling " << poll_amount << std::endl; // debug comment
        #endif

        // iterating all found fd's as long fd is within poll list
        // from here on we handle a single fd/client per iteration.
        // and where do i push fd's into the vector list?
        // -> process_ready_fd

        // double check fd (iterator) with every client_poll.fd
        for (size_t poll_index = 0; poll_index < irc_server.getPollFD().size(); poll_index++)
        {
        #if defined(DEBUG_BUILD) && DEBUG_BUILD
            std::cout << "for fd: " << poll_index << " iterator" << std::endl;
        #endif
            pollfd &client_poll = irc_server.getPollFD()[poll_index];

            /*
                client cleanup on quit/disconnect
               bug:
                on force disconnect (ctrl+c/window quit)
                invalid read errors
                due receive_message unregister/cleanup
            */
            ClientUser *client_for_current_fd = nullptr;
            if (client_poll.fd != irc_server.get_server_fd())
                client_for_current_fd = irc_server.getClientByFd(client_poll.fd);

            if (client_for_current_fd && client_for_current_fd->isToDisconnect())
            {
                int fd_to_remove = client_for_current_fd->get_ClientUser_fd();
                close(fd_to_remove);
                irc_server.unregisterClientFd(fd_to_remove);
                // swap-pop to remove current pollfd without invalidating iteration badly
                size_t last = irc_server.getPollFD().size() - 1;
                if (poll_index != last)
                    irc_server.getPollFD()[poll_index] = irc_server.getPollFD()[last];
                irc_server.getPollFD().pop_back();
                // step back so the next iteration processes the moved entry
                if (poll_index != 0) --poll_index;
                continue;
            }
            // bug till here.


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

                // waiting clients
                // receive_message()
                // handleClientInput -> dispatch -> commands
                int rc = process_ready_fd(irc_server, poll_index);
                client_poll.revents = 0;
                if (rc == -1)
                    continue;
            }

            // POLLOUT -> socket write check
            // set POLLOUT on outputbuffer to non empty, to tell the kernel to try to send from that fd
            if (client_poll.fd != irc_server.get_server_fd())
            {
                // if its not empty set pollout in .events.
                if (client_for_current_fd && !client_for_current_fd->get_outputBuffer().get_buffer().empty())
                    client_poll.events |= POLLOUT;

                // if pollout is set in .revents
                if (client_poll.revents & POLLOUT)
                {
                    // process the msg to send.
                    if (process_fd_ready_for_sending(irc_server, poll_index) < 0)
                    {
                        return -1;
                    }
                    client_poll.revents = 0;
                }

                // if outputBuffer is empty, &= ~POLLOUT
                if (!client_for_current_fd || client_for_current_fd->get_outputBuffer().get_buffer().empty())
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

    #if defined(DEBUG_BUILD) && DEBUG_BUILD
        std::cout << "max_clients: " << max_clients << std::endl; // debug print
    #endif

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

